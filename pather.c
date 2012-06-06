#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

struct node {
	char* val;
	struct node* next;
};

struct linked_list {
	struct node* head;
	struct node* tail;
	int size;
};

struct linked_list* create_linked_list(void)
{
	struct linked_list* list = malloc(sizeof(struct linked_list));

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;

	return list;
}

//add an item to the tail of the linked list
void add_item(char* item, struct linked_list* list)
{
	struct node* new_node = malloc(sizeof(struct node));
	new_node->val = item;
	new_node->next = NULL;

	if (list->head == NULL) {
		list->head = new_node;
		list->tail = new_node;
		list->size = 1;
	} else {
		list->tail->next = new_node;
		list->tail = new_node;
		list->size++;
	}
}

//removes the item at the specified position in the list
void remove_item(int pos, struct linked_list* list) 
{
	int cur_pos = 0;
	struct node* cur = list->head;
	struct node* prev = NULL;

	if (pos == 0) {
		list->head = list->head->next;
		free(cur);
		return;
	}

	while (cur != NULL) {
		if (cur_pos == pos) {
			prev->next = cur->next;
			free(cur);
			return;
		}

		cur_pos++;
		prev = cur;
		cur = cur->next;
	}
}

//converts all chars to lowercase and backslashes to foreslashes
void normalize(char* str)
{
	int x;
	int len = strlen(str);

	for (x = 0; x < len; x++) {
		if (str[x] == '/') {
			str[x] = '\\';
		} else {
			str[x] = tolower(str[x]);
		}
	}

	//trim the trailing slash if present
	if (str[len - 1] == '\\') {
		str[len - 1] = '\0';
	}
}

//return the index of the string in the linked list if present 
//or -1 otherwise. All strings are lowercased and backslashes 
//are converted to foreslashes for the comparision
int contains(char* find, struct linked_list* list)
{
	int find_pos = 0;
	struct node* cur = list->head;
	int node_buf_size = 128;
	char* normalized_find = malloc(strlen(find) + 1);
	char* normalized_node = malloc(node_buf_size);
	
	strcpy_s(normalized_find, strlen(find) + 1, find);
	strcpy(normalized_node, "");

	normalize(normalized_find);

	while (cur != NULL) {
		//realloc more space for the node value if needed
		if (strlen(cur->val) > node_buf_size) {
			node_buf_size = strlen(cur->val) + node_buf_size;
			normalized_node = realloc(
				normalized_node, 
				node_buf_size);
		}

		strcpy_s(normalized_node, node_buf_size, cur->val);
		normalize(normalized_node);

		if (strcmp(normalized_node, normalized_find) == 0) {
			return find_pos;
		}

		find_pos++;
		cur = cur->next;
	}

	free(normalized_find);
	free(normalized_node);
	return -1;
}

//joins a linked list into a string with the delimiter 
//char in between the elements and at the end
char* join(char* delim, struct linked_list* list)
{
	int char_count = 0;
	int buf_size = 128;
	char* buf = malloc(buf_size);
	struct node* cur_node = list->head;

	strcpy(buf, "");
	while (cur_node != NULL) {
		char_count = char_count + strlen(cur_node->val) + 1;

		if (char_count >= buf_size) {
			buf_size = char_count + buf_size;
			buf = realloc(buf, buf_size);
		}

		strcat_s(buf, buf_size, cur_node->val);
		strcat_s(buf, buf_size, delim);
		cur_node = cur_node->next;
	}

	return buf;
}

//split a string into a linked list using delim as the 
//delimiter string
struct linked_list* split(const char* delim, char* str)
{
	char* context;
	char* token;
	struct linked_list* list = create_linked_list();

	token = strtok_s(str, delim, &context);

	while (token != NULL) {
		add_item(token, list);
		token = strtok_s(NULL, delim, &context);
	}

	return list;
}

//grabs the system environment variable from the registry
//or return 0 on error
char* get_path(void)
{
	char* value_buf;
	HKEY hkey;
	LPDWORD size_of_reg_key;
	long open_result, query_size_result, query_result;
	
	open_result = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE, 
		"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 
		0,
		KEY_QUERY_VALUE,
		&hkey);

	if (open_result != ERROR_SUCCESS) {
		return 0;
	}

	query_size_result = RegGetValue(
		hkey, 
		NULL, 
		"Path", 
		RRF_RT_ANY,
		NULL,
		NULL,
		&size_of_reg_key);

	if (query_size_result != ERROR_SUCCESS) {
		RegCloseKey(hkey);
		return NULL;
	}

	size_of_reg_key++;
	value_buf = malloc((size_t)size_of_reg_key);

	query_result = RegGetValue(
		hkey, 
		NULL, 
		"Path", 
		RRF_RT_ANY, 
		NULL,
		value_buf, 
		&size_of_reg_key);

	RegCloseKey(hkey);
	
	if (query_result != ERROR_SUCCESS) {
		return NULL;
	}

	return value_buf;
}

int set_path(char* path)
{
	long open_result, set_result;
	HKEY hkey; 

	open_result = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE, 
		"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 
		0,
		KEY_SET_VALUE,
		&hkey);

	if (open_result != ERROR_SUCCESS) {
		return NULL;
	}

	set_result = RegSetValueEx(
		hkey,
		"Path",
		NULL,
		REG_EXPAND_SZ,
		(BYTE*) path,

		strlen(path));
	
	if (set_result != ERROR_SUCCESS) {
		RegCloseKey(hkey);
		return NULL;
	}

	RegFlushKey(hkey);
	RegCloseKey(hkey);

	return 1;
}

int WINAPI WinMain(
		HINSTANCE hInstance, 
		HINSTANCE hPrevInstance, 
		LPTSTR lpCmdLine, 
		int cmdShow)
{
	int remove, found;
	char* joined;
	char* path;
	struct linked_list* list;
	LRESULT refresh_result;

	path = get_path();
	if (path == NULL) {
		return 1;
	}

	list = split(";", _strdup(path));
	found = contains(lpCmdLine, list);

	if (found != -1) {
		remove = MessageBox(NULL, "This directory is already in your path. "
								  "Would you like to remove it?", 
								  "Remove path",
								  MB_YESNO);
		if (remove != IDYES) { 
			return 0;
		}

		remove_item(found, list);

	} else {
		add_item(lpCmdLine, list);
	}

	joined = join(";", list);
	if (set_path(joined) == NULL){
		return 1;
	}

	//notify everyone that the path has changed
	SendMessageTimeout(
		HWND_BROADCAST, 
		WM_SETTINGCHANGE, 
		0, 
		(LPARAM) "Environment", 
		SMTO_ABORTIFHUNG,
		1000, 
		NULL);

	return 0;
}
