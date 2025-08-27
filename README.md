# Clippers

Clippers is a lightweight tool for extracting and sorting URLs out of raw html bulk data in file format.
It is designed for console-based workflows on Linux (tested on Debian 12).

## Tools

- **collector**  
  - accepts and reads a raw HTML file
  - extracts all `<a href="...">` links
  - and outputs clean URL in http or https format

- **group**  
  - Reads URLs from stdin
  - groups them by domain
  - removes duplicates 
  - and writes each multi-URL domain into its own file
    inside a `clean/` folder. (appends to across runs)

- **harvest**
  - runs `HEAD` requests on all data

## Build
Install dependencies:
```bash
sudo apt update
sudo apt install g++ libgumbo-dev libcurl4-openssl-dev -y
mkdir clean
make
```
## Usage
```bash
./collector sample.htm | ./group
./harvest
```

Results will be stored in the `clean` folder locally created.
