#ifndef VFS_H
#define VFS_H

#include <time.h>

#define MAX_NAME_LEN 64
#define MAX_CHILDREN 128
#define BLOCK_SIZE 1024  // 1KB simulated block

typedef enum {
    FILE_TYPE,
    DIR_TYPE,
    SYMLINK_TYPE
} NodeType;

typedef struct Node {
    char name[MAX_NAME_LEN];
    NodeType type;
    char owner[32];
    int permissions;
    time_t created;
    time_t modified;
    struct Node* parent;
    struct Node* children[MAX_CHILDREN];
    int child_count;

    union {
        char* file_content;       // For FILE_TYPE
        struct Node* link_target; // For SYMLINK_TYPE
    } content;
} Node;

extern Node* root;
extern Node* current;

void init_fs();
void mkdir_vfs(const char* name);
void touch_vfs(const char* name);
void ls_vfs();
void cd_vfs(const char* name);
void pwd_vfs();
void rmdir_vfs(const char* name);
void rm_vfs(const char* name);
void cp_vfs(const char* src_name, const char* dest_name);
void mv_vfs(const char* src_name, const char* dest_name);
void chmod_vfs(const char* name, int mode);
void chown_vfs(const char* name, const char* owner);
void find_vfs(const char* name);
void ln_vfs(const char* target_name, const char* link_name);
void df_vfs();
void du_vfs(const char* name);
void save_fs(const char* filename);
void load_fs(const char* filename);

#endif
