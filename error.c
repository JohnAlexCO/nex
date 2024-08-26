void ok (char *src, char *message) {
    printf(txtG "[%s] " txtW "%s\n", src, message);
}
void error (char *src, char *message) {
    printf(txtR "[%s] " txtW "%s\n", src, message);
    exit(false);
}