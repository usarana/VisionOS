#include <common/math.hpp>
#include <cxx/cstdint>
#include <cxx/cstddef>

size_t strlen(const char* str) {
    size_t length = 0;
    while(str[length]) {
        length++;
    }
    return length;
}

char* strcpy(char* destination, const char* source) {
    if (destination == NULL) {
        return NULL;
    }
    char* ptr = destination;
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

char* strcat(char* destination, const char* source) {
    char* ptr = destination + strlen(destination);
    while (*source != '\0')
    {
        *ptr++ = *source++;
    }
    *ptr = '\0';
    return destination;
}

int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) { ++a; ++b; }
    return (int)(unsigned char)(*a) - (int)(unsigned char)(*b);
}

char* strstr(const char *str, const char *substring) {
  const char *a = str, *b = substring;
  for (;;) {
    if ( !*b ) return (char *) str;
    if ( !*a ) return NULL;
    if ( *a++ != *b++) { a = ++str; b = substring; }
  }
}

char* strchr(const char str[], char ch) {
    while (*str && *str != ch ) ++str;
    return (char *)(ch == *str ? str : NULL);  
}

void memcpy(void* dest, void* src, size_t n) {
    int i;
    char* src_char = (char *)src;
    char* dest_char = (char *)dest;
    for (i = 0; i < n; i++)
    {
        dest_char[i] = src_char[i];
    }
}

void memset(void* str, char ch, size_t n) {
  	  int i;
	  char* s = (char *) str;
	  for(i = 0; i < n; i++) {
            s[i] = ch;
    }
}

void memmove(void* dest, void* src, size_t n) {
    char *csrc = (char *)src;
    char *cdest = (char *)dest;
    char temp[n];
    for (int i=0; i<n; i++) {
        temp[i] = csrc[i];
    }
    for (int i=0; i<n; i++) {
        cdest[i] = temp[i];
    }
}

void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

char* int_to_string(int num) {
    bool isMinus = false;
    static char out[10];
    int g = 0;
    if (num != 0) {
        char temp[10];
        int i = 0;
        if (num < 0) {
            isMinus = true;
            num = -num;
        }
        if (num > 0); else { temp[i++] = '8'; num = -(num / 10); }
        while (num > 0) {
            temp[i++] = num % 10 + '0';
            num /= 10;
        }
        if (isMinus) {
            out[g] = '-';
            g++;
        }
        while (--i >= 0) {
            out[g] = temp[i];
            g++;
        }
        return out;
    }
    else {
        return "0";
    }
}

int string_to_int(char* str) {
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        res = res * 10 + str[i] - '0';
    }
    return res;
}

long oct_to_dec(int oct) {
    int dec = 0, temp = 0;

    while(oct != 0) {
        dec = dec + (oct % 10) * pow(8, temp);
        temp++;
        oct = oct / 10;
    }

    return dec;
}


