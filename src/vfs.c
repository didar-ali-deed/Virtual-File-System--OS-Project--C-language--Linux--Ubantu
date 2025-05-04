#include "../include/vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Node* root = NULL;
Node* current = NULL;

void format_permissions(int perm, char* out) {
    out[0] = (perm & 0400) ? 'r' : '-';
    out[1] = (perm & 0200) ? 'w' : '-';
    out[2] = (perm & 0100) ? 'x' : '-';
    out[3] = (perm & 0040) ? 'r' : '-';
    out[4] = (perm & 0020) ? 'w' : '-';
    out[5] = (perm & 0010) ? 'x' : '-';
    out[6] = (perm & 0004) ? 'r' : '-';
    out[7] = (perm & 0002) ? 'w' : '-';
    out[8] = (perm & 0001) ? 'x' : '-';
    out[9] = '\0';
}


void init_fs() {
    root = (Node*)malloc(sizeof(Node));
    strcpy(root->name, "/");
    root->type = DIR_TYPE;
    strcpy(root->owner, "root");
    root->permissions = 0755;
    root->created = root->modified = time(NULL);
    root->parent = NULL;
    root->child_count = 0;
    memset(&root->content, 0, sizeof(root->content));
    current = root;
}
void mkdir_vfs(const char* name) {
    if (current->child_count >= MAX_CHILDREN) {
        printf("Directory full!\n");
        return;
    }

    Node* dir = (Node*)malloc(sizeof(Node));
    strcpy(dir->name, name);
    dir->type = DIR_TYPE;
    strcpy(dir->owner, "user");
    dir->permissions = 0755;
    dir->created = dir->modified = time(NULL);
    dir->parent = current;
    dir->child_count = 0;
    memset(&dir->content, 0, sizeof(dir->content));


    current->children[current->child_count++] = dir;
}

void touch_vfs(const char* name) {
    if (current->child_count >= MAX_CHILDREN) {
        printf("Directory full!\n");
        return;
    }

    Node* file = (Node*)malloc(sizeof(Node));
    strcpy(file->name, name);
    file->type = FILE_TYPE;
    strcpy(file->owner, "user");
    file->permissions = 0644;
    file->created = file->modified = time(NULL);
    file->parent = current;
    file->child_count = 0;
    file->content.file_content = strdup("");

    current->children[current->child_count++] = file;
}

void ls_vfs() {
    for (int i = 0; i < current->child_count; i++) {
        Node* node = current->children[i];
        Node* display_node = node;

        char type = (node->type == DIR_TYPE) ? 'd' :
                    (node->type == SYMLINK_TYPE) ? 'l' : '-';

        // Dereference symlink if it points to a directory (for size/display)
        if (node->type == SYMLINK_TYPE) {
            Node* target = node->content.link_target;
            if (target && target->type == DIR_TYPE) {
                display_node = target;
            }
        }

        char perms[10];
        format_permissions(display_node->permissions, perms);

        char time_buf[20];
        struct tm* tm_info = localtime(&display_node->modified);
        strftime(time_buf, 20, "%b %d %H:%M", tm_info);

        printf("%c%s %s %s %s",
               type, perms, display_node->owner, time_buf, node->name);

        // Show symlink arrow
        if (node->type == SYMLINK_TYPE) {
            Node* target = node->content.link_target;
            printf(" -> %s", target ? target->name : "(null)");
        }

        printf("\n");
    }
}


void cd_vfs(const char* name) {
    if (strcmp(name, ".") == 0) return;

    if (strcmp(name, "..") == 0) {
        if (current->parent != NULL)
            current = current->parent;
        return;
    }

    const char* actual_name = name;

    if (strncmp(name, "./", 2) == 0) {
        actual_name = name + 2;
    }

    for (int i = 0; i < current->child_count; i++) {
        Node* child = current->children[i];

        if (strcmp(child->name, actual_name) == 0) {
            // If symbolic link to directory, follow it
            if (child->type == SYMLINK_TYPE) {
                Node* target = child->content.link_target;
                if (target->type == DIR_TYPE) {
                    current = target;
                    return;
                } else {
                    printf("Symlink is not a directory.\n");
                    return;
                }
            }

            if (child->type == DIR_TYPE) {
                current = child;
                return;
            } else {
                printf("%s is not a directory.\n", actual_name);
                return;
            }
        }
    }

    printf("Directory not found: %s\n", name);
}



void remove_node(Node* node) {
    if (node->type == DIR_TYPE) {
        for (int i = 0; i < node->child_count; i++) {
            remove_node(node->children[i]);
        }
    }

    if (node->type == FILE_TYPE)
    free(node->content.file_content);
    free(node);
}

void remove_child(Node* parent, int index) {
    for (int i = index; i < parent->child_count - 1; i++) {
        parent->children[i] = parent->children[i + 1];
    }
    parent->child_count--;
}

void rmdir_vfs(const char* name) {
    for (int i = 0; i < current->child_count; i++) {
        Node* child = current->children[i];
        if (child->type == DIR_TYPE && strcmp(child->name, name) == 0) {
            if (child->child_count > 0) {
                printf("Directory not empty: %s\n", name);
                return;
            }

            remove_node(child);
            remove_child(current, i);
            return;
        }
    }

    printf("Directory not found: %s\n", name);
}

void rm_vfs(const char* name) {
    for (int i = 0; i < current->child_count; i++) {
        Node* child = current->children[i];
        if (strcmp(child->name, name) == 0) {
            remove_node(child);
            remove_child(current, i);
            return;
        }
    }

    printf("File or directory not found: %s\n", name);
}

Node* clone_node(Node* original) {
    Node* copy = (Node*)malloc(sizeof(Node));
    strcpy(copy->name, original->name);
    copy->type = original->type;
    strcpy(copy->owner, original->owner);
    copy->permissions = original->permissions;
    copy->created = copy->modified = time(NULL);
    copy->child_count = 0;
    copy->parent = NULL;

    if (original->type == FILE_TYPE) {
        copy->content.file_content = strdup(original->content.file_content);

    } else {
        memset(&copy->content, 0, sizeof(copy->content));
        for (int i = 0; i < original->child_count; i++) {
            Node* child_copy = clone_node(original->children[i]);
            child_copy->parent = copy;
            copy->children[copy->child_count++] = child_copy;
        }
    }

    return copy;
}

void cp_vfs(const char* src_name, const char* dest_name) {
    for (int i = 0; i < current->child_count; i++) {
        Node* src = current->children[i];
        if (strcmp(src->name, src_name) == 0) {
            Node* copy = clone_node(src);
            strcpy(copy->name, dest_name);
            copy->parent = current;
            current->children[current->child_count++] = copy;
            return;
        }
    }

    printf("Source not found: %s\n", src_name);
}

void mv_vfs(const char* src_name, const char* dest_name) {
    for (int i = 0; i < current->child_count; i++) {
        Node* src = current->children[i];
        if (strcmp(src->name, src_name) == 0) {
            strcpy(src->name, dest_name);
            return;
        }
    }

    printf("Source not found: %s\n", src_name);
}
void chmod_vfs(const char* name, int mode) {
    for (int i = 0; i < current->child_count; i++) {
        Node* child = current->children[i];
        if (strcmp(child->name, name) == 0) {
            child->permissions = mode;
            child->modified = time(NULL);
            return;
        }
    }

    printf("File or directory not found: %s\n", name);
}

void chown_vfs(const char* name, const char* owner) {
    for (int i = 0; i < current->child_count; i++) {
        Node* child = current->children[i];
        if (strcmp(child->name, name) == 0) {
            strcpy(child->owner, owner);
            child->modified = time(NULL);
            return;
        }
    }

    printf("File or directory not found: %s\n", name);
}

void pwd_vfs() {
    char path[1024] = "";
    Node* temp = current;

    while (temp != NULL) {
        char segment[MAX_NAME_LEN + 2];
        sprintf(segment, "/%s", temp->name);
        memmove(path + strlen(segment), path, strlen(path) + 1);
        memcpy(path, segment, strlen(segment));
        temp = temp->parent;
    }

    printf("%s\n", path);
}

void find_recursive(Node* node, const char* name, char* path) {
    char current_path[1024];
    sprintf(current_path, "%s/%s", path, node->name);

    if (strcmp(node->name, name) == 0) {
        printf("Found: %s\n", current_path);
    }

    if (node->type == DIR_TYPE) {
        for (int i = 0; i < node->child_count; i++) {
            find_recursive(node->children[i], name, current_path);
        }
    }
}

void find_vfs(const char* name) {
    find_recursive(current, name, "");
}

void ln_vfs(const char* target_name, const char* link_name) {
    for (int i = 0; i < current->child_count; i++) {
        Node* target = current->children[i];
        if (strcmp(target->name, target_name) == 0) {
            if (current->child_count >= MAX_CHILDREN) {
                printf("Directory full!\n");
                return;
            }

            Node* link = (Node*)malloc(sizeof(Node));
            strcpy(link->name, link_name);
            link->type = SYMLINK_TYPE;
            strcpy(link->owner, "user");
            link->permissions = 0777;
            link->created = link->modified = time(NULL);
            link->parent = current;
            link->child_count = 0;
            link->content.link_target = target;
            current->children[current->child_count++] = link;
            return;
        }
    }

    printf("Target not found: %s\n", target_name);
}
int calculate_size(Node* node) {
    int size = BLOCK_SIZE;

    if (node->type == FILE_TYPE && node->content.file_content != NULL) {
        return BLOCK_SIZE;
    }

    if (node->type == DIR_TYPE) {
        for (int i = 0; i < node->child_count; i++) {
            size += calculate_size(node->children[i]);
        }
    }

    return size;
}


void df_vfs() {
    int used = calculate_size(root);
    printf("Total disk: 1MB\n");
    printf("Used: %.2f KB\n", used / 1024.0);
    printf("Free: %.2f KB\n", (1024.0 - (used / 1024.0)));
}

void du_vfs(const char* name) {
    for (int i = 0; i < current->child_count; i++) {
        Node* node = current->children[i];
        if (strcmp(node->name, name) == 0) {
            int size = calculate_size(node);
            printf("Size of %s: %.2f KB\n", name, size / 1024.0);
            return;
        }
    }

    printf("File or directory not found: %s\n", name);
}

void write_node(FILE* fp, Node* node) {
    fprintf(fp, "%s %d %s %o %ld %d\n",
            node->name, node->type, node->owner,
            node->permissions, node->modified, node->child_count);

    if (node->type == FILE_TYPE && node->content.file_content != NULL)
        fprintf(fp, "%s\n", node->content.file_content);
    else if (node->type == SYMLINK_TYPE && node->content.link_target != NULL)
        fprintf(fp, "%s\n", node->content.link_target->name);
    for (int i = 0; i < node->child_count; i++) {
        write_node(fp, node->children[i]);
    }
    fprintf(fp, "#\n"); // marks end of children
}

void save_fs(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Failed to save file system.\n");
        return;
    }
    write_node(fp, root);
    fclose(fp);
    printf("File system saved to %s\n", filename);
}

Node* read_node(FILE* fp, Node* parent) {
    char name[MAX_NAME_LEN];
    int type;
    char owner[32];
    int perm;
    long mod_time;
    int children;
    
    if (fscanf(fp, "%s %d %s %o %ld %d\n", name, &type, owner, &perm, &mod_time, &children) != 6)
        return NULL;

    Node* node = (Node*)malloc(sizeof(Node));
    strcpy(node->name, name);
    node->type = type;
    strcpy(node->owner, owner);
    node->permissions = perm;
    node->modified = mod_time;
    node->created = mod_time;
    node->parent = parent;
    node->child_count = 0;

    if (type == FILE_TYPE) {
        char buffer[1024];
        fgets(buffer, sizeof(buffer), fp);
        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline
        node->content.file_content = strdup(buffer);
    } else if (type == SYMLINK_TYPE) {
        char target_name[64];
        fgets(target_name, sizeof(target_name), fp);
        target_name[strcspn(target_name, "\n")] = '\0';
        // Symbolic link targets will be resolved after full tree is loaded
        node->content.file_content = strdup(target_name); // temporary storage
    } else {
        memset(&node->content, 0, sizeof(node->content));
    }

    while (1) {
        long pos = ftell(fp);
        char check = fgetc(fp);
        if (check == '#') {
            fscanf(fp, "\n");
            break;
        } else {
            fseek(fp, pos, SEEK_SET);
            Node* child = read_node(fp, node);
            if (child)
                node->children[node->child_count++] = child;
        }
    }

    return node;
}

void resolve_symlinks(Node* node) {
    if (node->type == SYMLINK_TYPE && node->content.file_content != NULL) {
        char* name = node->content.file_content;
    
        for (int i = 0; i < node->parent->child_count; i++) {
            Node* sibling = node->parent->children[i];
            if (strcmp(sibling->name, name) == 0) {
                free(node->content.file_content);
                node->content.link_target = sibling;
                break;
            }
        }
    }
    

    for (int i = 0; i < node->child_count; i++)
        resolve_symlinks(node->children[i]);
}

void load_fs(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Failed to load file system.\n");
        return;
    }
    root = read_node(fp, NULL);
    current = root;
    resolve_symlinks(root);
    fclose(fp);
    printf("File system loaded from %s\n", filename);
}

