import i3ipc
import time
import subprocess
import logging
from Xlib import X, display
from Xlib.protocol import event

# Set up logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def create_xterm_windows(i3):
    logger.info("Starting create_xterm_windows function")

    # Connect to the X server
    d = display.Display()
    logger.debug(f"Connected to X display: {d.get_display_name()}")

    xterm_configs = [
        {
            "name": "top_xterm",
            "command": "htop --filter sshd",
            "size": (795, 107),
            "position": (15, 15),
            "font_size": 7,
            "border_color": "green"
        },
        {
            "name": "middle_xterm",
            "command": "ranger",
            "size": (800, 635),
            "position": (15, 138),
            "font_size": 8,
            "border_color": "green"
        },
        {
            "name": "right_xterm",
            "command": "bash -c 'neofetch; exec bash'",
            "size": (432, 758),
            "position": (831, 15),
            "font_size": 7,
            "border_color": "green"
        }
    ]

    for config in xterm_configs:
        logger.info(f"Creating xterm window: {config['name']}")
        cmd = [
            "xterm",
            "-bg", "black",
            "-fg", "green",
            "-fa", "Monospace",
            "-fs", str(config['font_size']),
            "-b", "0",
            "-bc",
            "-cr", "green",
            "-sl", "1000",
            "+sb",
            "-name", config['name']
        ]

        if config['command']:
            cmd.extend(["-e", config['command']])

        process = subprocess.Popen(cmd)

        # Wait for the window to be created
        time.sleep(1)

        # Find the window
        window = None
        for attempt in range(10):
            tree = i3.get_tree()
            windows = tree.find_instanced(config['name'])
            if windows:
                window = windows[0]
                break
            time.sleep(0.5)

        if window:
            logger.info(f"Window {config['name']} found in i3 tree")

            # Configure the window
            i3.command(f'[instance="{config["name"]}"] floating enable, resize set {config["size"][0]} {config["size"][1]}, move position {config["position"][0]} {config["position"][1]}')

            # Add custom border
            x_window = d.create_resource_object('window', window.window)

            # Set border width
            x_window.configure(border_width=2)

            # Set border color
            colormap = d.screen().default_colormap
            border_color = colormap.alloc_named_color(config['border_color']).pixel
            x_window.change_attributes(border_pixel=border_color)

            # Redraw the window
            x_window.clear_area()

            logger.info(f"Custom border added to {config['name']}")
        else:
            logger.warning(f"Window {config['name']} not found in i3 tree")

    # Flush the display
    d.flush()
    logger.debug("Display flushed")

    logger.info("Window creation and configuration complete")

    # Keep the script running to maintain the custom borders
    logger.info("Entering main loop. Press Ctrl+C to exit.")
    while True:
        event = d.next_event()
        logger.debug(f"Received event: {event}")

if __name__ == "__main__":
    logger.info("Script started")
    i3 = i3ipc.Connection()
    logger.debug("i3ipc connection established")
    create_xterm_windows(i3)
