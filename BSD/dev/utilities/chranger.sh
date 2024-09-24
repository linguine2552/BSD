#!/bin/bash

print_header() {
    echo "====================================="
    echo "$1"
    echo "====================================="
}

print_command() {
    printf "%-20s %s\n" "$1" "$2"
}

print_header "Ranger Quick Reference"

echo "Basic Navigation:"
print_command "h, j, k, l" "Move left, down, up, right"
print_command "gg" "Go to top"
print_command "G" "Go to bottom"
print_command "H, M, L" "Go to top, middle, bottom of viewport"

echo ""
print_header "File Operations"
print_command "space" "Select file"
print_command "v" "Invert selection"
print_command "cw" "Rename file"
print_command "yy" "Yank (copy) file"
print_command "dd" "Cut file"
print_command "pp" "Paste file"

echo ""
print_header "Tabs and Windows"
print_command "gn, ^N" "Create new tab"
print_command "gt, gT" "Go to next/previous tab"
print_command "gc" "Close current tab"
print_command "^W" "Close current window"

echo ""
print_header "Search and Filter"
print_command "/" "Search for files"
print_command "n" "Jump to next match"
print_command "N" "Jump to previous match"
print_command "f" "Filter stack"

echo ""
print_header "File Preview"
print_command "i" "Display file"
print_command "E" "Edit file"

echo ""
print_header "Bookmarks"
print_command "m<key>" "Create bookmark"
print_command "`<key>" "Go to bookmark"

echo ""
print_header "Shell"
print_command "!" "Execute shell command"
print_command "@" "Execute shell command on selected files"
print_command "S" "Open shell in current directory"

echo ""
echo "For more information, use 'ranger --help' or 'man ranger'"
