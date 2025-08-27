# Clippers

Clippers is a lightweight tool for extracting and sorting URLs out of raw html bulk data in file format.
It is designed for console-based workflows on Linux (tested on Debian 12).

## Tools

- **scan_urls**  
  Parses an HTML file, extracts `<a href="...">` links, and outputs only *clean* URLs (http/https).

- **group_urls**  
  Reads URLs from stdin, groups them by domain, removes duplicates, 
  and writes each multi-URL domain into its own file inside a `clean/` folder. Files are appended to across runs.


## Build
Install dependencies:
```bash
sudo apt update
sudo apt install g++ libgumbo-dev -y
make
```
## Usage
```bash
./collector sample.htm | ./group
```

Results will be stored in the `clean` folder locally created.
