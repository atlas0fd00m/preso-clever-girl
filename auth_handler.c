/*
 * auth_handler.c — GrrCON 2026 Demo 2: Vulnerability Hunting
 *
 * Source code for the agent to analyze. Contains multiple vulnerability
 * classes that the agent must identify, classify, and patch:
 *
 *   1. strcpy(session_token, input)       — buffer overflow   (Severity: 9)
 *   2. for (i=0; i<=count; i++)           — off-by-one        (Severity: 4)
 *   3. sprintf(query, "...%s", id)        — SQL injection     (Severity: 10)
 *   4. strcmp(password, hash)             — timing attack     (Severity: 6)
 *
 * The agent reads this file, finds all four bugs, classifies each by
 * severity, and writes patches. Same workflow works for code review
 * (blue team) and exploit discovery (red team).
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>

#define MAX_TOKEN_LEN 256
#define MAX_QUERY_LEN 1024

/* VULNERABILITY #1: Buffer overflow
 * strcpy copies without bounds checking. If input > MAX_TOKEN_LEN,
 * this overflows session_token on the stack.
 * Fix: use strncpy or snprintf with bounds.
 */
char *generate_session_token(const char *input) {
    static char session_token[MAX_TOKEN_LEN];
    strcpy(session_token, input);  /* BUG: no bounds check */
    return session_token;
}

/* VULNERABILITY #2: Off-by-one error
 * Loop condition is <= count instead of < count.
 * Writes one element past the end of the array.
 * Fix: change <= to <.
 */
int *allocate_user_slots(int count) {
    int *slots = malloc(count * sizeof(int));
    int i;
    for (i = 0; i <= count; i++) {   /* BUG: should be < count */
        slots[i] = 0;
    }
    return slots;
}

/* VULNERABILITY #3: SQL injection
 * User-supplied id is concatenated directly into the query string.
 * An attacker can inject SQL: id = "1 OR 1=1 --"
 * Fix: use parameterized queries (mysql_stmt_prepare).
 */
int fetch_user_by_id(MYSQL *conn, const char *id, char *result, size_t result_len) {
    char query[MAX_QUERY_LEN];
    sprintf(query, "SELECT username FROM users WHERE id = %s", id);  /* BUG: SQL injection */
    
    if (mysql_query(conn, query) != 0) {
        return -1;
    }
    
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        return -1;
    }
    
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) {
        mysql_free_result(res);
        return -1;
    }
    
    strncpy(result, row[0], result_len - 1);
    result[result_len - 1] = '\0';
    mysql_free_result(res);
    return 0;
}

/* VULNERABILITY #4: Timing attack
 * strcmp returns as soon as it finds a mismatch, which leaks
 * information about how many characters match via timing.
 * Fix: use a constant-time comparison function.
 */
int verify_password(const char *password, const char *stored_hash) {
    if (strcmp(password, stored_hash) == 0) {  /* BUG: timing side-channel */
        return 1;  /* authenticated */
    }
    return 0;  /* rejected */
}

/* --- Entry point for testing --- */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <session_input>\n", argv[0]);
        return 1;
    }
    
    char *token = generate_session_token(argv[1]);
    printf("Session token: %s\n", token);
    
    int *slots = allocate_user_slots(10);
    free(slots);
    
    return 0;
}