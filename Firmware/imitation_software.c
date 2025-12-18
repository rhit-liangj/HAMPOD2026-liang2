#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "hampod_firm_packet.h"

#define INPUT_PIPE "Firmware_i"
#define OUTPUT_PIPE "Firmware_o"

const char* keypad_names[] = {
    "1", "2", "3", "A", "4", "5", "6", "B", "7", "8", "9", "C", "POINT", "0", "POUND", "D"
};
const char* DTMF_names[] = {
    "DTMF1", "DTMF2", "DTMF3", "DTMFA", "DTMF4", "DTMF5", "DTMF6", "DTMFB", "DTMF7", "DTMF8", 
    "DTMF9", "DTMFC", "DTMFASTERISK", "DTMF0", "DTMFPOUND", "DTMFD"
};

int index_getter(char keypad_input){
    /* Map keypad character to array index matching keypad_names/DTMF_names layout:
     * 0:'1', 1:'2', 2:'3', 3:'A', 4:'4', 5:'5', 6:'6', 7:'B',
     * 8:'7', 9:'8', 10:'9', 11:'C', 12:'*'(POINT), 13:'0', 14:'#'(POUND), 15:'D'
     */
    switch(keypad_input) {
        case '1': return 0;
        case '2': return 1;
        case '3': return 2;
        case 'A': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case 'B': return 7;
        case '7': return 8;
        case '8': return 9;
        case '9': return 10;
        case 'C': return 11;
        case '*': return 12;  /* POINT/ASTERISK */
        case '0': return 13;
        case '#': return 14;  /* POUND */
        case 'D': return 15;
        default:  return -1;
    }
}
int input_pipe;
int output_pipe;
void send_packet(Inst_packet* packet){
    printf("Message = %s\n", packet->data);
    
    // Serialize header explicitly to match Firmware reader
    write(output_pipe, &packet->type, sizeof(Packet_type));
    write(output_pipe, &packet->data_len, sizeof(unsigned short));
    write(output_pipe, &packet->tag, sizeof(unsigned short));

    unsigned char buffer[256];
    memcpy(buffer, packet->data, packet->data_len);
    printf("Message = %s\n", (char*)buffer);
    write(output_pipe, buffer, packet->data_len);
}

Inst_packet* read_from_pipe(){
    unsigned char buffer[256];
    Packet_type type;
    unsigned short size;
    unsigned short tag;
    ssize_t bytes_read;
    
    bytes_read = read(input_pipe, &type, sizeof(Packet_type));
    if (bytes_read <= 0) {
        printf("ERROR: Pipe closed or read error (type)\n");
        return NULL;
    }
    
    bytes_read = read(input_pipe, &size, sizeof(unsigned short));
    if (bytes_read <= 0) {
        printf("ERROR: Pipe closed or read error (size)\n");
        return NULL;
    }
    
    bytes_read = read(input_pipe, &tag, sizeof(unsigned short));
    if (bytes_read <= 0) {
        printf("ERROR: Pipe closed or read error (tag)\n");
        return NULL;
    }
    
    bytes_read = read(input_pipe, buffer, size);
    if (bytes_read <= 0) {
        printf("ERROR: Pipe closed or read error (data)\n");
        return NULL;
    }
    
    Inst_packet* temp = create_inst_packet(type, size, buffer, tag);
    return temp;
}

int main(){
    printf("Hampod Software Emulation Tool For interacting with the hardware\n");
    printf("Size of type = %lu\n", sizeof(Packet_type));
    printf("Connecting to Firmware_o\n");
    input_pipe = open(OUTPUT_PIPE, O_RDONLY);
    if(input_pipe == -1){
        perror("open");
        exit(-1);
    }
    printf("Attempting to connect to Firmware_i\n");
    // output_pipe; // Statement with no effect
    for(int i = 0; i < 1000; i++){
        output_pipe = open(INPUT_PIPE, O_WRONLY);
        printf("Attempt %d/1000\r", i);
        if(output_pipe != -1){
            break;
        }
        usleep(10000); // Wait 10ms between attempts
    }
    if(output_pipe == -1){
        printf("\nUnsuccessful Connection\n");
        perror("open");
        exit(1);
    }
    printf("\nSuccessful connection to Firmware_i\n");
    while(1) {
        printf("Packet attempt\n");
        int mode = 0; //keypad = 0: DTMF = 1
        char new_data = AUDIO;
        Packet_type keypad = KEYPAD;
        unsigned char keypad_msg = 'r';
        char len = 1;
        char msg[] = "sThis is a keypad and audio integration test for the firmware. Press * to toggle DTMF mode.";
        Inst_packet* temp = create_inst_packet(new_data, strlen(msg) + 1, (unsigned char*)msg, 0);
        send_packet(temp);
        Inst_packet* audio_packet = read_from_pipe();
        if (audio_packet == NULL) {
            printf("ERROR: Failed to read audio response, exiting\n");
            break;
        }
        destroy_inst_packet(&audio_packet);
        while(1) { //sue me
            temp = create_inst_packet(keypad, len, &keypad_msg, 0);
            send_packet(temp);
            destroy_inst_packet(&temp);
            Inst_packet* keypad_packet = read_from_pipe();
            if (keypad_packet == NULL) {
                printf("ERROR: Failed to read keypad response, exiting\n");
                goto cleanup;
            }
            if(keypad_packet->type != KEYPAD) {
                printf("bruh (got type %d, expected KEYPAD=%d)\n", keypad_packet->type, KEYPAD);
                destroy_inst_packet(&keypad_packet);
                continue;
            }
            char keypad_moment = keypad_packet->data[0];
            printf("keypad says %x (%c)\n", keypad_moment, keypad_moment);
            
            /* Handle mode toggle on '*' key */
            if(keypad_moment == '*'){
                mode ^= 1;
                printf("Mode toggled to %s\n", mode ? "DTMF" : "Normal");
                destroy_inst_packet(&keypad_packet);
                continue;
            }
            
            /* Skip no-key events */
            if(keypad_moment == 0xFF || keypad_moment == '-'){
                destroy_inst_packet(&keypad_packet);
                continue;
            }
            
            /* Convert character to array index */
            int idx = index_getter(keypad_moment);
            if(idx < 0 || idx >= 16) {
                printf("Unknown/invalid key: %c (0x%x)\n", keypad_moment, keypad_moment);
                destroy_inst_packet(&keypad_packet);
                continue;
            }
            
            char buffer[256];
            if(mode == 0) {
                printf("Playing: %s\n", keypad_names[idx]);
                strcpy(buffer, "ppregen_audio/");
                strcat(buffer, keypad_names[idx]);
                temp = create_inst_packet(new_data, strlen(buffer) + 1, (unsigned char*)buffer, 0);
                send_packet(temp);
                destroy_inst_packet(&temp);
                audio_packet = read_from_pipe();
                if (audio_packet != NULL) {
                    destroy_inst_packet(&audio_packet);
                }
            } else {
                printf("Playing: %s\n", DTMF_names[idx]);
                strcpy(buffer, "ppregen_audio/");
                strcat(buffer, DTMF_names[idx]);
                temp = create_inst_packet(new_data, strlen(buffer) + 1, (unsigned char*)buffer, 0);
                send_packet(temp);
                destroy_inst_packet(&temp);
                audio_packet = read_from_pipe();
                if (audio_packet != NULL) {
                    destroy_inst_packet(&audio_packet);
                }
            }
            destroy_inst_packet(&keypad_packet);
            usleep(16670);
        }
        
    }

cleanup:
    printf("Closing pipes...\n");
    close(input_pipe);
    close(output_pipe);
    return 0;
}
