# Harvest v2.0 - Enhanced URL Validation Tool

A robust, rate-limited URL validation tool that checks the presence of URLs using HEAD requests while being respectful to both local and remote systems.

## 🚀 Key Improvements in v2.0

### Rate Limiting & Flood Protection
- **Host-based rate limiting**: Prevents flooding specific hosts
- **Configurable delays**: Adjustable minimum and maximum delays between requests
- **Adaptive timing**: Automatically adjusts delays based on response success/failure
- **Exponential backoff**: Intelligent retry logic with increasing delays

### Performance & Robustness
- **Connection pooling**: Reuses CURL handles for better performance
- **Concurrent connection control**: Configurable maximum concurrent connections
- **Better error handling**: Comprehensive retry logic and error reporting
- **Progress tracking**: Real-time progress bar and statistics

### System Protection
- **Resource management**: Prevents overwhelming local system resources
- **Remote server protection**: Respects server capabilities and rate limits
- **Configurable timeouts**: Adjustable connection and request timeouts
- **SSL handling**: Configurable SSL verification options

## 📋 Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- libcurl development libraries
- POSIX-compliant system (Linux, macOS, BSD)

## 🛠️ Installation

### 1. Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y libcurl4-openssl-dev build-essential
```

**CentOS/RHEL/Fedora:**
```bash
sudo yum install -y libcurl-devel gcc-c++ make
# or for newer versions:
sudo dnf install -y libcurl-devel gcc-c++ make
```

**Arch Linux:**
```bash
sudo pacman -S --needed curl base-devel
```

### 2. Build the Application

```bash
make
# or for optimized release build:
make release
```

### 3. Install (Optional)

```bash
sudo make install
```

## ⚙️ Configuration

The application can be configured through the `harvest.conf` file. Key settings include:

```ini
# Rate Limiting
min_delay_ms=100          # Minimum delay between requests (ms)
max_delay_ms=2000         # Maximum delay between requests (ms)

# Timeouts
timeout_seconds=10        # Request timeout
connect_timeout=5         # Connection timeout

# Retry Logic
max_retries=3             # Maximum retry attempts
backoff_multiplier=2      # Exponential backoff multiplier

# Connection Pool
max_concurrent=5          # Maximum concurrent connections
```

## 🚀 Usage

### Basic Usage

```bash
# Run with default settings
./harvest

# Run with custom configuration
./harvest --config harvest.conf

# Check dependencies
make check-deps
```

### Directory Structure

The application expects a `clean/` directory containing text files with URLs (one per line):

```
clean/
├── domains1.txt
├── domains2.txt
└── domains3.txt
```

### Output

The application provides:
- Real-time progress bar
- Per-file validation results
- Comprehensive statistics
- Atomic file updates (using temporary files)

## 📊 Features

### Rate Limiting
- **Host-based**: Each host gets its own rate limit
- **Configurable**: Adjust delays via configuration file
- **Adaptive**: Increases delays for failed requests

### Connection Management
- **Pooling**: Reuses CURL handles efficiently
- **Concurrency control**: Prevents resource exhaustion
- **Timeout handling**: Configurable connection and request timeouts

### Error Handling
- **Retry logic**: Automatic retry with exponential backoff
- **Graceful degradation**: Continues processing on individual failures
- **Comprehensive logging**: Detailed error reporting

### Progress Tracking
- **Real-time updates**: Live progress bar
- **Statistics**: Request counts, success rates, timing
- **File-level reporting**: Per-file validation results

## 🔧 Advanced Configuration

### Host-Specific Rate Limiting

Add host-specific delays in `harvest.conf`:

```ini
# Custom delays for specific hosts
example.com=500
api.github.com=1000
cdn.example.com=200
```

### SSL Configuration

```ini
# SSL verification settings
ssl_verify_peer=0         # Disable peer verification
ssl_verify_host=0         # Disable hostname verification
```

### Logging Options

```ini
# Logging configuration
log_level=2               # Verbose logging
log_file=harvest.log      # Log to file
```

## 📈 Performance Tuning

### For High-Volume Processing

```ini
# Aggressive settings (use with caution)
min_delay_ms=50
max_concurrent=10
timeout_seconds=5
max_retries=2
```

### For Conservative Processing

```ini
# Conservative settings (respectful to servers)
min_delay_ms=500
max_concurrent=3
timeout_seconds=15
max_retries=5
```

## 🐛 Troubleshooting

### Common Issues

1. **libcurl not found**: Install development libraries
2. **Compilation errors**: Ensure C++17 support
3. **Permission denied**: Check directory permissions
4. **Rate limiting**: Adjust delays in configuration

### Debug Mode

```bash
make debug
./harvest
```

### Dependency Check

```bash
make check-deps
```

## 📝 License

This project is part of the Clippers toolset for URL processing and validation.

## 🤝 Contributing

To contribute improvements:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📞 Support

For issues and questions:
- Check the troubleshooting section
- Review configuration options
- Ensure all dependencies are installed
- Test with debug build for detailed output
