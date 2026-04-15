# rdc-cli Command Reference

Use the CLI help to get current command information:

```bash
rdc --help                    # List all available commands
rdc <command> --help          # Detailed flags and arguments for a command
rdc --version                 # Version info
```

Examples:
```bash
rdc capture --help            # Capture options
rdc shader --help             # Shader inspection options
rdc debug pixel --help        # Pixel debugging options
rdc --help | grep <keyword>   # Search for a command by name
```

For context on when and why to use specific commands, see the debugging recipes and workflow instructions.