# Remote Install Script for Windows (PowerShell)
# Usage: .\remote_install.ps1 "<commit_message>"

param(
    [Parameter(Mandatory=$true)]
    [string]$CommitMessage
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Step 1: Committing and Pushing local changes..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# Add all changes
git add .

# Commit with the provided message
git commit -m $CommitMessage

# Push to the default remote/branch (usually origin main)
git push

# Check if push was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Git push failed. Aborting." -ForegroundColor Red
    exit 1
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Step 2: Connecting to remote to Pull and Make..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# SSH into the remote machine, pull changes, and run make in the Software directory
ssh waynesr@HAMPOD.local "cd ~/HAMPOD2026/Software && echo 'Pulling changes...' && git pull && echo 'Running make...' && make"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Remote build failed." -ForegroundColor Red
    exit 1
}

Write-Host "========================================" -ForegroundColor Green
Write-Host "Remote install process complete." -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
