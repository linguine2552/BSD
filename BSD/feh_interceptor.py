#!/usr/bin/env py3

import subprocess
import sys
import os

def main(window_id):
    print(f"Feh interceptor running in window {window_id}")
    while True:
        try:
            command = input("Enter a command (or 'exit' to quit): ")
            if command.lower() == 'exit':
                break
            if command.startswith('feh'):
                # Modify the feh command to display in this window
                modified_command = f"feh --window-id {window_id} " + command[4:]
                print(f"Running modified command: {modified_command}")
                subprocess.run(modified_command, shell=True)
            else:
                print("Not a feh command. Ignoring.")
        except EOFError:
            # Handle Ctrl+D (EOF) gracefully
            print("\nReceived EOF. Exiting.")
            break
        except KeyboardInterrupt:
            # Handle Ctrl+C gracefully
            print("\nReceived interrupt. Exiting.")
            break

if __name__ == "__main__":
    if len(sys.argv) > 1:
        window_id = sys.argv[1]
        main(window_id)
    else:
        print("Please provide the window ID as an argument.")
