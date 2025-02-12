#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <errno.h>

#define DEFAULT_INTERVAL 15
#define BUFFER_SIZE 1024

bool show_time = true;
bool use_color = true;
int interval = DEFAULT_INTERVAL;
bool log_enabled = false;
char log_dir[BUFFER_SIZE] = "";
char log_file_path[BUFFER_SIZE] = "";

void print_help() {
    printf("Usage: retkonfirmi-c [OPTIONS] <website URL>\n\n");
    printf("This program checks the status of a website and prints if it is up or down.\n\n");
    printf("Options:\n");
    printf("  --hide_time        Hide the timestamp in the output.\n");
    printf("  --monochrome       Disable colored output.\n");
    printf("  --interval <sec>   Set the check interval in seconds (default: 15 seconds).\n");
    printf("  --log              Enable logging of messages to a log file.\n");
    printf("  --logdir <path>    Specify a custom directory for logs (default: current directory).\n");
    printf("  --help             Display this help message and exit.\n\n");
    printf("Example:\n");
    printf("  ./retkonfirmi-c --interval 30 --log --logdir /tmp www.example.com\n");
}

void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void log_message(const char *message) {
    if (log_enabled && strlen(log_file_path) > 0) {
        FILE *log_file = fopen(log_file_path, "a");
        if (log_file) {
            fprintf(log_file, "%s\n", message);
            fclose(log_file);
        } else {
            perror("Error opening log file");
        }
    }
}

void print_status(const char *website, bool up) {
    char message[BUFFER_SIZE];
    char timestamp[BUFFER_SIZE] = "";
    
    if (show_time) {
        get_timestamp(timestamp, sizeof(timestamp));
    }

    if (use_color) {
        if (up) {
            snprintf(message, sizeof(message), "%s%s <Website is Up>\033[0m", timestamp, strlen(timestamp) > 0 ? " " : "");
            printf("\033[32m%s\n", message);  // Green
        } else {
            snprintf(message, sizeof(message), "%s%s <Website is Down>\033[0m", timestamp, strlen(timestamp) > 0 ? " " : "");
            printf("\033[31m%s\n", message);  // Red
        }
    } else {
        snprintf(message, sizeof(message), "%s%s <Website is %s>", timestamp, strlen(timestamp) > 0 ? " " : "", up ? "Up" : "Down");
        printf("%s\n", message);
    }

    log_message(message);
}

bool check_website(const char *url) {
    CURL *curl;
    CURLcode res;
    bool is_up = false;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);  // Don't download body
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);  // Follow redirects
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code >= 200 && response_code < 300) {
                is_up = true;
            }
        }
        curl_easy_cleanup(curl);
    }
    return is_up;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    char *url = NULL;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--hide_time") == 0) {
            show_time = false;
        } else if (strcmp(argv[i], "--monochrome") == 0) {
            use_color = false;
        } else if (strcmp(argv[i], "--interval") == 0) {
            if (i + 1 < argc) {
                interval = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: --interval requires a value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--log") == 0) {
            log_enabled = true;
        } else if (strcmp(argv[i], "--logdir") == 0) {
            if (i + 1 < argc) {
                strncpy(log_dir, argv[++i], sizeof(log_dir) - 1);
            } else {
                fprintf(stderr, "Error: --logdir requires a directory path\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        } else if (argv[i][0] != '-') {
            url = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    if (url == NULL) {
        fprintf(stderr, "Error: No website URL provided\n");
        return 1;
    }

    // Add protocol if not provided by user
    char full_url[BUFFER_SIZE];
    if (strncmp(url, "http", 4) != 0) {
        snprintf(full_url, sizeof(full_url), "http://%s", url);
    } else {
        strncpy(full_url, url, sizeof(full_url));
    }

    // Setup logging
    if (log_enabled) {
        if (strlen(log_dir) == 0) {
            strncpy(log_dir, ".", sizeof(log_dir) - 1);  // Default to current directory
        }
        struct stat st;
        if (stat(log_dir, &st) == -1) {
            if (errno == ENOENT) {
                fprintf(stderr, "Error: Log directory does not exist: %s\n", log_dir);
                return 1;
            }
        }
        char log_filename[BUFFER_SIZE];
        char timestamp[BUFFER_SIZE];
        get_timestamp(timestamp, sizeof(timestamp));
        snprintf(log_filename, sizeof(log_filename), "%s/.%s_retkonfirmi-c_%s.log", log_dir, timestamp, url);
        strncpy(log_file_path, log_filename, sizeof(log_file_path) - 1);
    }

    // Start checking the website status in a loop
    while (1) {
        bool website_up = check_website(full_url);
        print_status(full_url, website_up);
        sleep(interval);
    }

    return 0;
}
