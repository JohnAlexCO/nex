int file_read(char *filename, char *buffer, int max) {
    FILE *fp = fopen(filename, "rb");
    if ( fp == NULL ) { return false; }
    int cursor = 0; char c; while(cursor < max -1) {
      c = fgetc(fp);
      if( feof(fp) ) { break; }
      buffer[cursor] = c; cursor++;
   } fclose(fp); return cursor;
}

int file_write (char *filename, char *buffer, int max) {
    FILE *fp = fopen(filename, "w+");
    if ( fp == NULL ) { return false; }
    int cursor; for (cursor = 0; cursor < max; cursor++) {
        if (fputc(buffer[cursor], fp) == EOF) { fclose(fp); return false; }
    } fclose(fp); return cursor;
}