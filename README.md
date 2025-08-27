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

## Dry run
```
@atx:~/Clipper$ ./collector sample.htm | ./group
@atx:~/Clipper$ ./harvest
[*] Processing file: clean/myactivity.google.com.txt
  -> Checking: https://myactivity.google.com/privacyadvisor/search?utm_source=googlemenu&fg=1 ... OK (200)
  -> Checking: https://myactivity.google.com/product/search?utm_source=google&hl=en-VI&fg=1 ... OK (200)
[*] Updating file: clean/myactivity.google.com.txt (2 valid URLs)
[*] Processing file: clean/policies.google.com.txt
  -> Checking: https://policies.google.com/privacy?hl=en-VI&fg=1 ... OK (200)
  -> Checking: https://policies.google.com/terms?hl=en-VI&fg=1 ... OK (200)
[*] Updating file: clean/policies.google.com.txt (2 valid URLs)
[*] Processing file: clean/www.google.com.txt
  -> Checking: https://www.google.com/advanced_search?hl=en-VI&fg=1 ... OK (200)
  -> Checking: https://www.google.com/imghp?hl=en&authuser=0&ogbl ... OK (200)
  -> Checking: https://www.google.com/intl/en_vi/ads/?subid=ww-ww-et-g-awa-a-g_hpafoot1_1!o2&utm_source=google.com&utm_medium=referral&utm_campaign=google_hpafooter&fg=1 ... OK (200)
  -> Checking: https://www.google.com/preferences?hl=en-VI&fg=1 ... OK (200)
  -> Checking: https://www.google.com/services/?subid=ww-ww-et-g-awa-a-g_hpbfoot1_1!o2&utm_source=google.com&utm_medium=referral&utm_campaign=google_hpbfooter&fg=1 ... OK (200)
[*] Updating file: clean/www.google.com.txt (5 valid URLs)
[*] Harvest complete.
```

Results will be stored in the `clean` folder locally created.
