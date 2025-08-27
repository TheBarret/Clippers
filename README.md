# Clippers

Clippers is a lightweight tool for extracting and sorting URLs out of raw html bulk data in file format.
It is designed for console-based workflows on Linux (tested on Debian 12).

## Tools

- **collector**  
  - accepts and reads a raw HTML file
  - extracts all `<a href="...">` http or https format links
  - outputs clean URL to stdout

- **group**  
  - Reads URLs from stdin
  - groups them by domain
  - removes duplicates 
  - writes each multi-URL domain into its own file
    (output folder `clean`, will only append data)

- **harvest**
  - runs `HEAD` requests on all data
  - updates files generated from `group` data


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
  -> Checking: https://google.com/search?utm_source=googlemenu&fg=1 ... OK (200)
  -> Checking: https://google.com/search?utm_source=google&hl=en-VI&fg=1 ... OK (200)
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

Remark: `Harvest` will retain the file structure and save all data locally.

## License

Clippers is released under the **GPL-3.0 License**.  

## Disclaimer

These tools are intended for **legal and ethical use only**.  
The authors are not responsible for any misuse.  
Users are fully responsible for complying with laws, regulations, and terms of service when using Clippers.  
While the software is open-source and freely available for the public,
it is provided **as-is** for research, educational, and legitimate website service management purposes.
