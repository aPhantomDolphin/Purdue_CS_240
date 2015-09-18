
#include <stdio.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <time.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_RESPONSE (10 * 1024)

int port;
char * host;
char * sport;
char * userName;
char * password;
char * argument;
GtkWidget * table;
GtkWidget * window;
GtkWidget * listRoom;
GtkWidget * listUser;
GtkWidget * labelName;
GtkWidget * labelPass;
GtkWidget * tree_view;
GtkWidget * chatMessages;
GtkListStore * list_rooms;
GtkListStore * list_users;
GtkWidget * entryEnterRoom;
GtkWidget * entryLeaveRoom;
GtkWidget * entryCreateRoom;
GtkWidget * labelRoomNameEntered;
static char buffer[256];
char * response = (char *)malloc(100 * sizeof(char));
char * newMessage = (char *)malloc(200 * sizeof(char));
const char * newRoom = (char *)malloc(100 * sizeof(char));
const char * newUser = (char *)malloc(100 * sizeof(char));
const char * newPassword = (char *)malloc(100 * sizeof(char));



int open_client_socket(char * host, int port) {
	// Initialize socket address structure
	struct	sockaddr_in socketAddress;
	
	// Clear sockaddr structure
	memset((char *)&socketAddress,0,sizeof(socketAddress));
	
	// Set family to Internet 
	socketAddress.sin_family = AF_INET;
	
	// Set port
	socketAddress.sin_port = htons((u_short)port);
	
	// Get host table entry for this host
	struct	hostent	*ptrh = gethostbyname(host);
	if ( ptrh == NULL ) {
		perror("gethostbyname");
		exit(1);
	}
	
	// Copy the host ip address to socket address structure
	memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);
	
	// Get TCP transport protocol entry
	struct	protoent *ptrp = getprotobyname("tcp");
	if ( ptrp == NULL ) {
	perror("getprotobyname");
	exit(1);
	}
	
	// Create a tcp socket
	int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sock < 0) {
	perror("socket");
	exit(1);
	}
	
	// Connect the socket to the specified server
	if (connect(sock, (struct sockaddr *)&socketAddress,
		sizeof(socketAddress)) < 0) {
	perror("connect");
	exit(1);
	}
	
	return sock;
}

int sendCommand(char * host, int port, char * command, const char * userName, 
	const char * password, const char * argument, char * message, char * response){
	
	int sock = open_client_socket(host, port);

	if (sock<0) {
	return 0;
	}

	// Send command
	write(sock, command, strlen(command));
	write(sock, " ", 1);
	write(sock, userName, strlen(userName));
	write(sock, " ", 1);
	write(sock, password, strlen(password));
	write(sock, " ", 1);
	write(sock, argument, strlen(argument));
	write(sock, " ", 1);
	write(sock, message, strlen(message));

	write(sock, "\r\n",2);

	//Print copy to stdout
	write(1, command, strlen(command));
	write(1, " ", 1);
	write(1, userName, strlen(userName));
	write(1, " ", 1);
	write(1, password, strlen(password));
	write(1, " ", 1);
	write(1, argument, strlen(argument));
	write(1, " ", 1);
	write(1, message, strlen(message));
	write(1, "\r\n",2);

	// Keep reading until connection is closed or MAX_REPONSE
		int n = 0;
		int len = 0;
		while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
				len += n;
		}
		response[len]=0;
		printf("response:\n%s\n", response);

	close(sock);

	return 1;
}

static GtkWidget * create_list( const char * titleColumn, GtkListStore * model ) {
	GtkWidget *scrolled_window;
	GtkCellRenderer *cell;
	GtkTreeViewColumn *column;

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	tree_view = gtk_tree_view_new ();
	gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
	gtk_widget_show (tree_view);

	 
	cell = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (titleColumn, cell, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column));

	return scrolled_window;
}

void enter_room_by_choice(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * col, gpointer userdata) {
	GtkTreeModel * model;
	GtkTreeIter iter;
	model = gtk_tree_view_get_model(treeview);
	if (gtk_tree_model_get_iter(model, &iter, path)) {
		gchar * name;
		gtk_tree_model_get(model, &iter, 0, &name, -1);
		//strcpy(strdup(newRoom), name);
		newRoom = strdup(name);

		sendCommand(host, port, "ENTER-ROOM", newUser, newPassword, newRoom, "NULL", response);

		if (strcmp(response, "OK\r\n") == 0) {
			g_print("System: %s is entering room %s.", newUser, newRoom);
			sendCommand(host, port, "SEND-MESSAGE", newUser, newPassword, newRoom, "ENTERED ROOM ", response);
		}	

		g_free(name);
	}
	gtk_label_set_markup(GTK_LABEL(labelRoomNameEntered), newRoom);	
}

static GtkWidget * get_list_rooms() {
	GtkTreeIter iter;
	GtkListStore * sign_list_rooms;
	tree_view = gtk_tree_view_new ();
	// GtkTreeModel * model = gtk_tree_view_get_model(tree_view);
	// create_list("Users In Room", GTK_TREE_MODEL(model));
	create_list("Users In Room", sign_list_rooms);
	sendCommand(host, port, "LIST-ROOMS", newUser, newPassword, strdup("NULL"), "NULL", response);
	if (response != NULL) {
		gtk_widget_hide (listRoom);	
		sign_list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
		char * roomName;
		roomName = strtok(response, "\r\n");
		while(roomName != NULL) {
			gchar *msg = g_strdup_printf ("%s", roomName);
			gtk_list_store_append (GTK_LIST_STORE (sign_list_rooms), &iter);
			gtk_list_store_set (GTK_LIST_STORE (sign_list_rooms), &iter,0, msg,-1);
			g_free (msg);
			roomName = strtok(NULL, "\r\n");
		}
		listRoom = create_list ("Rooms", sign_list_rooms);
		gtk_table_attach_defaults (GTK_TABLE (table), listRoom, 3, 6, 0, 3);
		gtk_widget_show (listRoom);
	}
	// tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(sign_list_rooms));
	g_signal_connect(tree_view, "row-activated", G_CALLBACK(enter_room_by_choice), NULL);
}

void get_list_users() {
	GtkTreeIter iter;
	GtkListStore * sign_list_users;
	sendCommand(host, port, "GET-USERS-IN-ROOM", newUser, newPassword, newRoom, strdup("NULL"), response);
	if (strcmp(response, "DENIED\r\n") != 0) {
		gtk_widget_hide (listUser);	
		sign_list_users = gtk_list_store_new (1, G_TYPE_STRING);
		char * userNameInRoom;
		userNameInRoom = strtok(response, "\r\n");
		while(userNameInRoom != NULL) {
			gchar *msg = g_strdup_printf ("%s", userNameInRoom);
			gtk_list_store_append (GTK_LIST_STORE (sign_list_users), &iter);
			gtk_list_store_set (GTK_LIST_STORE (sign_list_users), 
							&iter,
								0, msg,
							-1);
			g_free (msg);
			userNameInRoom = strtok(NULL, "\r\n");
		}
		listUser = create_list ("Users In Room", sign_list_users);
		gtk_table_attach_defaults (GTK_TABLE (table), listUser, 0, 3, 0, 3);
		gtk_widget_show (listUser);	
	}
	else if (strcmp(response, "ERROR (Wrong password)\r\n") == 0) {
		sign_list_users = gtk_list_store_new (1, G_TYPE_STRING);
		gchar *msg = g_strdup_printf ("No User In Room");
		gtk_list_store_append (GTK_LIST_STORE (sign_list_users), &iter);
		gtk_list_store_set (GTK_LIST_STORE (sign_list_users), 
							&iter,
								0, msg,
							-1);
		listUser = create_list ("Users In Room", sign_list_users);
		gtk_table_attach_defaults (GTK_TABLE (table), listUser, 0, 3, 0, 3);
		gtk_widget_show (listUser);	
	}
	else {
		sign_list_users = gtk_list_store_new (1, G_TYPE_STRING);
		gchar *msg = g_strdup_printf ("No User In Room");
		gtk_list_store_append (GTK_LIST_STORE (sign_list_users), &iter);
		gtk_list_store_set (GTK_LIST_STORE (sign_list_users), 
							&iter,
								0, msg,
							-1);
		listUser = create_list ("Users In Room", sign_list_users);
		gtk_table_attach_defaults (GTK_TABLE (table), listUser, 0, 3, 0, 3);
		gtk_widget_show (listUser);	
	}
}

static void insert_text( GtkTextBuffer *buffer, const char * initialText ) {
	GtkTextIter iter;
	gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
	gtk_text_buffer_insert (buffer, &iter, initialText,strlen(initialText));
}
	 
static GtkWidget * create_text(char * initialText) {
	GtkWidget * view = gtk_text_view_new ();
	GtkTextBuffer * buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	//gtk_text_buffer_set_modified (buffer, true);

	GtkWidget * scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	gtk_container_add (GTK_CONTAINER (scrolled_window), view);
	// insert_text (buffer, initialText);
	gtk_text_buffer_set_text (buffer, initialText, strlen(initialText));

	PangoFontDescription * font_desc = pango_font_description_from_string ("Serif 15");
	gtk_widget_modify_font (view, font_desc);
	pango_font_description_free (font_desc);

	gtk_widget_show_all (scrolled_window);

	return scrolled_window;
}

void get_message() {
	gtk_widget_hide(chatMessages);
	if (strcmp(newRoom, "NULL") != 0)
		sendCommand(host, port, "GET-MESSAGES", newUser, newPassword, "0", strdup(newRoom), response);
	if (strcmp(response, "DENIED\r\n") != 0)
		chatMessages = create_text (response);
	else 
		chatMessages = create_text("You are not in any chat room.\r\n");
	gtk_table_attach_defaults (GTK_TABLE (table), chatMessages, 0, 6, 3, 7);
}

static gboolean on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	cairo_t *cr;

	cr = gdk_cairo_create(widget->window);

	cairo_move_to(cr, 30, 30);
	cairo_show_text(cr, buffer);

	cairo_destroy(cr);

	return FALSE;
}

static gboolean time_handler(GtkWidget *widget) {
	if (widget->window == NULL) 
		return FALSE;
	
	time_t curtime;
	struct tm *loctime;

	curtime = time(NULL);
	loctime = localtime(&curtime);
	strftime(buffer, 256, "%T", loctime);

	gtk_widget_queue_draw(widget);

	return TRUE;
}

static gboolean get_update(GtkWidget *widget) {
	if (widget->window == NULL) 
		return FALSE;
	
	time_t curtime;
	struct tm *loctime;

	curtime = time(NULL);
	loctime = localtime(&curtime);
	strftime(buffer, 256, "%T", loctime);

	gtk_widget_queue_draw(widget);
	get_list_rooms();
	get_list_users();
	get_message();

	return TRUE;
}

void update_timely() {
	g_timeout_add(2000, (GSourceFunc) get_update, (gpointer) window);
}

void create_new_room() {
	newRoom = gtk_entry_get_text(GTK_ENTRY(entryCreateRoom));
	GtkWidget *window_success;
	window_success = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window_success), "Create Room");
	g_signal_connect (window_success, "destroy",
				G_CALLBACK (gtk_window_iconify), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window_success), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window_success), 300, 150);

	GtkWidget *table = gtk_table_new (6, 8, TRUE);
	gtk_container_add (GTK_CONTAINER (window_success), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 2);
	gtk_table_set_col_spacings(GTK_TABLE (table), 2);
	gtk_widget_show (table);

	sendCommand(host, port, "CREATE-ROOM", newUser, newPassword, newRoom, "NULL", response);

	GtkWidget *continue_button = gtk_button_new_with_label (response);
	//GtkWidget *continue_button = gtk_button_new_with_label ("Success!");
	gtk_table_attach_defaults(GTK_TABLE (table), continue_button, 1, 7, 2, 4);
	gtk_widget_show (continue_button);
	// g_signal_connect_swapped(continue_button, "clicked", G_CALLBACK(hide_window), window_success);
	if (strcmp(response, "OK\r\n") == 0){
		g_signal_connect(continue_button, "clicked", G_CALLBACK(get_list_rooms), NULL);
		g_signal_connect(continue_button, "clicked", G_CALLBACK(get_list_users), NULL);
		g_signal_connect(continue_button, "clicked", G_CALLBACK(get_update), NULL);
	}
	g_signal_connect_swapped(continue_button, "clicked", (GCallback) gtk_widget_hide, window_success);

	gtk_widget_show (window_success);
}

void create_room() {
	GtkWidget *window_create_room;
	window_create_room = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window_create_room), "Create Room");
	g_signal_connect (window_create_room, "destroy",
				G_CALLBACK (gtk_window_iconify), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window_create_room), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window_create_room), 350, 300);

	GtkWidget *table = gtk_table_new (12, 7, TRUE);
	gtk_container_add (GTK_CONTAINER (window_create_room), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 5);
	gtk_table_set_col_spacings(GTK_TABLE (table), 5);
	gtk_widget_show (table);

	GtkWidget *labelCreateRoom = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelCreateRoom), "<big>Create Room</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelCreateRoom, 1, 6, 0, 1); 
	gtk_widget_show (labelCreateRoom);

	GtkWidget *labelRoom = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelRoom), "<big>Room Name</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelRoom, 1, 6, 3, 4); 
	gtk_widget_show (labelRoom);

	entryCreateRoom = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryCreateRoom), "ex: gintama");
	gtk_table_attach_defaults(GTK_TABLE (table), entryCreateRoom, 1, 6, 5, 6); 
	gtk_widget_show (entryCreateRoom);

	//newRoom = gtk_entry_get_text(GTK_ENTRY(entryRoom));

	GtkWidget *continue_button = gtk_button_new_with_label ("Continue");
	gtk_table_attach_defaults(GTK_TABLE (table), continue_button, 4, 6, 9, 10);
	gtk_widget_show (continue_button);
	g_signal_connect(continue_button, "clicked", G_CALLBACK(create_new_room), NULL);
	g_signal_connect_swapped(continue_button, "clicked", (GCallback) gtk_widget_hide, window_create_room);

	GtkWidget *cancel_button = gtk_button_new_with_label ("cancel");
	gtk_table_attach_defaults(GTK_TABLE (table), cancel_button, 1, 3, 9, 10);
	gtk_widget_show (cancel_button);
	g_signal_connect_swapped(cancel_button, "clicked", (GCallback) gtk_widget_hide, window_create_room);

	gtk_widget_show (window_create_room);
}

void enter_new_room() {
	newRoom = gtk_entry_get_text(GTK_ENTRY(entryEnterRoom));
	GtkWidget *window_success;
	window_success = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window_success), "Success");
	g_signal_connect (window_success, "destroy",
				G_CALLBACK (gtk_window_iconify), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window_success), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window_success), 300, 150);

	GtkWidget *table = gtk_table_new (6, 8, TRUE);
	gtk_container_add (GTK_CONTAINER (window_success), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 2);
	gtk_table_set_col_spacings(GTK_TABLE (table), 2);
	gtk_widget_show (table);

	sendCommand(host, port, "ENTER-ROOM", newUser, newPassword, newRoom, "NULL", response);
	sendCommand(host, port, "SEND-MESSAGE", newUser, newPassword, newRoom, "ENTERED ROOM", response);


	GtkWidget *continue_button = gtk_button_new_with_label (response);
	gtk_table_attach_defaults(GTK_TABLE (table), continue_button, 1, 7, 2, 4);
	gtk_widget_show (continue_button);
	if (strcmp(response, "OK\r\n") == 0) {
		g_signal_connect(continue_button, "clicked", G_CALLBACK(get_list_rooms), NULL);
		g_signal_connect(continue_button, "clicked", G_CALLBACK(get_list_users), NULL);
		g_signal_connect(continue_button, "clicked", G_CALLBACK(get_update), NULL);
	}

	gtk_widget_show (window_success);
	gtk_label_set_markup(GTK_LABEL(labelRoomNameEntered), newRoom);
	g_signal_connect_swapped(continue_button, "clicked", (GCallback) gtk_widget_hide, window_success);
}

void enter_room() {
	GtkWidget *window_enter_room;
	window_enter_room = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window_enter_room), "Enter Room");
	g_signal_connect (window_enter_room, "destroy",
				G_CALLBACK (gtk_window_iconify), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window_enter_room), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window_enter_room), 350, 300);

	GtkWidget *table = gtk_table_new (12, 7, TRUE);
	gtk_container_add (GTK_CONTAINER (window_enter_room), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 5);
	gtk_table_set_col_spacings(GTK_TABLE (table), 5);
	gtk_widget_show (table);

	GtkWidget *labelEnterRoom = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelEnterRoom), "<big>Enter Room</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelEnterRoom, 1, 6, 0, 1); 
	gtk_widget_show (labelEnterRoom);

	GtkWidget *labelRoom = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelRoom), "<big>Room Name</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelRoom, 1, 6, 3, 4); 
	gtk_widget_show (labelRoom);

	entryEnterRoom = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryEnterRoom), "ex: gintama");
	gtk_table_attach_defaults(GTK_TABLE (table), entryEnterRoom, 1, 6, 5, 6); 
	gtk_widget_show (entryEnterRoom);

	GtkWidget *continue_button = gtk_button_new_with_label ("Continue");
	gtk_table_attach_defaults(GTK_TABLE (table), continue_button, 4, 6, 9, 10);
	gtk_widget_show (continue_button);
	g_signal_connect(continue_button, "clicked", G_CALLBACK(enter_new_room), NULL);
	g_signal_connect_swapped(continue_button, "clicked", (GCallback) gtk_widget_hide, window_enter_room);

	GtkWidget *cancel_button = gtk_button_new_with_label ("cancel");
	gtk_table_attach_defaults(GTK_TABLE (table), cancel_button, 1, 3, 9, 10);
	gtk_widget_show (cancel_button);
	g_signal_connect_swapped(cancel_button, "clicked", (GCallback) gtk_widget_hide, window_enter_room);

	gtk_widget_show (window_enter_room);
}

void leave_current_room() {
	GtkWidget *window_success;
	window_success = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window_success), "Success");
	g_signal_connect (window_success, "destroy",
				G_CALLBACK (gtk_window_iconify), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window_success), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window_success), 300, 150);

	GtkWidget *table = gtk_table_new (6, 8, TRUE);
	gtk_container_add (GTK_CONTAINER (window_success), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 2);
	gtk_table_set_col_spacings(GTK_TABLE (table), 2);
	gtk_widget_show (table);

	sendCommand(host, port, "SEND-MESSAGE", newUser, newPassword, newRoom, "IS LEAVING ROOM", response);
	sendCommand(host, port, "LEAVE-ROOM", newUser, newPassword, newRoom, "NULL", response);

	GtkWidget *continue_button = gtk_button_new_with_label (response);
	gtk_table_attach_defaults(GTK_TABLE (table), continue_button, 1, 7, 2, 4);
	gtk_widget_show (continue_button);
	if (strcmp(response, "OK\r\n") == 0) {
		g_signal_connect(continue_button, "clicked", G_CALLBACK(get_list_rooms), NULL);
		g_signal_connect(continue_button, "clicked", G_CALLBACK(get_list_users), NULL);
		g_signal_connect(continue_button, "clicked", G_CALLBACK(get_update), NULL);
	}
	gtk_widget_show (window_success);
	gtk_label_set_markup(GTK_LABEL(labelRoomNameEntered), "Room Entered");
	g_signal_connect_swapped(continue_button, "clicked", (GCallback) gtk_widget_hide, window_success);
}

void leave_room() {
	GtkWidget *window_leave_room;
	window_leave_room = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window_leave_room), "Leave Room");
	g_signal_connect (window_leave_room, "destroy",
				G_CALLBACK (gtk_window_iconify), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window_leave_room), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window_leave_room), 350, 300);

	GtkWidget *table = gtk_table_new (12, 7, TRUE);
	gtk_container_add (GTK_CONTAINER (window_leave_room), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 5);
	gtk_table_set_col_spacings(GTK_TABLE (table), 5);
	gtk_widget_show (table);

	GtkWidget *labelLeaveRoom = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelLeaveRoom), "<big>Leave Room</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelLeaveRoom, 1, 6, 0, 1); 
	gtk_widget_show (labelLeaveRoom);

	GtkWidget *labelRoom = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelRoom), "<big>Room Name</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelRoom, 1, 6, 3, 4); 
	gtk_widget_show (labelRoom);

	GtkWidget *labelRoomName = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelRoomName), newRoom);
	gtk_table_attach_defaults(GTK_TABLE (table), labelRoomName, 1, 6, 5, 6); 
	gtk_widget_show (labelRoomName);
	// entryLeaveRoom = gtk_entry_new();
	// gtk_entry_set_text(GTK_ENTRY(entryLeaveRoom), "ex: gintama");
	// gtk_table_attach_defaults(GTK_TABLE (table), entryLeaveRoom, 1, 6, 5, 6); 
	// gtk_widget_show (entryLeaveRoom);

	GtkWidget *continue_button = gtk_button_new_with_label ("Continue");
	gtk_table_attach_defaults(GTK_TABLE (table), continue_button, 4, 6, 9, 10);
	gtk_widget_show (continue_button);
	g_signal_connect(continue_button, "clicked", G_CALLBACK(leave_current_room), NULL);
	g_signal_connect_swapped(continue_button, "clicked", (GCallback) gtk_widget_hide, window_leave_room);

	GtkWidget *cancel_button = gtk_button_new_with_label ("cancel");
	gtk_table_attach_defaults(GTK_TABLE (table), cancel_button, 1, 3, 9, 10);
	gtk_widget_show (cancel_button);
	g_signal_connect_swapped(cancel_button, "clicked", (GCallback) gtk_widget_hide, window_leave_room);

	gtk_widget_show (window_leave_room);
}

void add_new_user() {
	GtkWidget *window_success;
	window_success = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window_success), "Sign Up");
	g_signal_connect (window_success, "destroy",
				G_CALLBACK (gtk_window_iconify), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window_success), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window_success), 300, 150);

	GtkWidget *tableSuccess = gtk_table_new (6, 8, TRUE);
	gtk_container_add (GTK_CONTAINER (window_success), tableSuccess);
	gtk_table_set_row_spacings(GTK_TABLE (tableSuccess), 2);
	gtk_table_set_col_spacings(GTK_TABLE (tableSuccess), 2);
	gtk_widget_show (tableSuccess);

	char * response = (char *)malloc(100 * sizeof(char));
	sendCommand(host, port, "ADD-USER", newUser, newPassword, "NULL", "NULL", response);
	if (strcmp(response, "DENIED\r\n") == 0)
		response = strdup("Log In Success!");
	else 
		response = strdup("Sign Up Success!");
	GtkWidget *continue_button = gtk_button_new_with_label (response);
	gtk_table_attach_defaults(GTK_TABLE (tableSuccess), continue_button, 1, 7, 2, 4);

	// labelRoomNameEntered = gtk_label_new(NULL);
	// gtk_label_set_markup(GTK_LABEL(labelRoomNameEntered), "<big>Room Entered</big>");
	// gtk_table_attach_defaults(GTK_TABLE (table), labelRoomNameEntered, 6, 8, 4, 5); 
	// gtk_widget_show (labelRoomNameEntered);

	gtk_widget_show (continue_button);
	g_signal_connect_swapped(continue_button, "clicked", (GCallback) gtk_widget_hide, window_success);
	g_signal_connect(continue_button, "clicked", G_CALLBACK(get_list_rooms), NULL);
	g_signal_connect(continue_button, "clicked", G_CALLBACK(get_list_users), NULL);
	g_signal_connect(continue_button, "clicked", G_CALLBACK(get_update), NULL);
	g_signal_connect(continue_button, "clicked", G_CALLBACK(update_timely), NULL);

	gtk_label_set_markup(GTK_LABEL(labelName), newUser);
	gtk_label_set_markup(GTK_LABEL(labelPass), newPassword);

	gtk_widget_show (window_success);
}

void sign_up(GtkWidget * button) {
	GtkWidget *window_sign_up;
	window_sign_up = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window_sign_up), "Sign Up");
	g_signal_connect (window_sign_up, "destroy",
				G_CALLBACK (gtk_window_iconify), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window_sign_up), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window_sign_up), 350, 300);

	GtkWidget *table = gtk_table_new (10, 7, TRUE);
	gtk_container_add (GTK_CONTAINER (window_sign_up), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 5);
	gtk_table_set_col_spacings(GTK_TABLE (table), 5);
	gtk_widget_show (table);

	GtkWidget *labelSignUp = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelSignUp), "<big>Sign up / Log In</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelSignUp, 1, 6, 0, 1); 
	gtk_widget_show (labelSignUp);

	GtkWidget *label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), "<big>User Name</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), label, 1, 6, 2, 3); 
	gtk_widget_show (label);

	GtkWidget * entryName = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryName), "ex: guo268");
	gtk_table_attach_defaults(GTK_TABLE (table), entryName, 1, 6, 3, 4); 
	gtk_widget_show (entryName);

	GtkWidget *labelPassWord = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelPassWord), "<big>Password</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelPassWord, 1, 6, 5, 6); 
	gtk_widget_show (labelPassWord);

	GtkWidget * entryPassword = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryPassword), "ex: 123456");
	gtk_table_attach_defaults(GTK_TABLE (table), entryPassword, 1, 6, 6, 7); 
	gtk_widget_show (entryPassword);

	newUser = gtk_entry_get_text(GTK_ENTRY(entryName));
	newPassword = gtk_entry_get_text(GTK_ENTRY(entryPassword));

	GtkWidget *continue_button = gtk_button_new_with_label ("Continue");
	gtk_table_attach_defaults(GTK_TABLE (table), continue_button, 4, 6, 8, 9);
	gtk_widget_show (continue_button);
	g_signal_connect(continue_button, "clicked", G_CALLBACK(add_new_user), NULL);
	g_signal_connect_swapped(continue_button, "clicked", (GCallback) gtk_widget_hide, window_sign_up);
	g_signal_connect_swapped(continue_button, "clicked", (GCallback) gtk_widget_hide, button);

	GtkWidget *cancel_button = gtk_button_new_with_label ("cancel");
	gtk_table_attach_defaults(GTK_TABLE (table), cancel_button, 1, 3, 8, 9);
	gtk_widget_show (cancel_button);
	g_signal_connect_swapped(cancel_button, "clicked", (GCallback) gtk_widget_hide, window_sign_up);

	gtk_widget_show (window_sign_up);
}

void send_message(GtkWidget * entry) {
	const char * sendMessage;
	sendMessage = gtk_entry_get_text(GTK_ENTRY(entry));
	newMessage = strdup(sendMessage);
	if (strcmp(strdup(newRoom), "NULL") != 0)
		sendCommand(host, port, "SEND-MESSAGE", newUser, newPassword, newRoom,newMessage, response);
}

void printUsage() {
	printf("Usage: test-talk-server host port\n");
	exit(1);
}

void update_list_rooms() {
	GtkTreeIter iter;
	int i;
	/* Add some messages to the window */
	for (i = 0; i < 10; i++) {
		gchar *msg = g_strdup_printf ("Room %d", i);
		gtk_list_store_append (GTK_LIST_STORE (list_rooms), &iter);
		gtk_list_store_set (GTK_LIST_STORE (list_rooms), 
						&iter,
							0, msg,
						-1);
	 g_free (msg);
	}
}

void update_list_users() {
	GtkTreeIter iter;
	int i;

	/* Add some messages to the window */
	for (i = 0; i < 10; i++) {
		gchar *msg = g_strdup_printf ("User %d", i);
		gtk_list_store_append (GTK_LIST_STORE (list_users), &iter);
		gtk_list_store_set (GTK_LIST_STORE (list_users), 
						&iter,
							0, msg,
						-1);
	g_free (msg);
	}
}

int main(int argc, char *argv[] ) {

	char * command;
	
	if (argc < 3) {
		printUsage();
	}

	host = argv[1];
	sport = argv[2];

	sscanf(sport, "%d", &port);

	newRoom = "NULL";

	gtk_init (&argc, &argv);
	 
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "Internet Relay Chat");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window), 1000 , 600);

	table = gtk_table_new (10, 6, TRUE);
	gtk_container_add (GTK_CONTAINER (window), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 5);
	gtk_table_set_col_spacings(GTK_TABLE (table), 5);
	gtk_widget_show (table);

	//Add lists of users. 
	list_users = gtk_list_store_new (1, G_TYPE_STRING);
	update_list_users();
	listUser = create_list ("Users In Room", list_users);
	gtk_table_attach_defaults (GTK_TABLE (table), listUser, 0, 3, 0, 3);
	gtk_widget_show (listUser);

	// Add list of rooms. Use columns 0 to 4 (exclusive) and rows 0 to 4 (exclusive)
	list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
	update_list_rooms();
	listRoom = create_list ("Rooms", list_rooms);
	gtk_table_attach_defaults (GTK_TABLE (table), listRoom, 3, 6, 0, 3);
	gtk_widget_show (listRoom);	

	chatMessages = create_text ("Welcome to Internet Relay Chat!\n");
	gtk_table_attach_defaults (GTK_TABLE (table), chatMessages, 0, 6, 3, 7);
	gtk_widget_show (chatMessages);

	// Add create room button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
	GtkWidget *create_room_button = gtk_button_new_with_label ("Create Room");
	gtk_table_attach_defaults(GTK_TABLE (table), create_room_button, 0, 2, 7, 8); 
	gtk_widget_show (create_room_button);
	g_signal_connect(create_room_button, "clicked", G_CALLBACK(create_room), NULL);

	// Add enter room button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
	GtkWidget *enter_room_button = gtk_button_new_with_label ("Enter Room");
	gtk_table_attach_defaults(GTK_TABLE (table), enter_room_button, 2, 4, 7, 8); 
	gtk_widget_show (enter_room_button);
	g_signal_connect(enter_room_button, "clicked", G_CALLBACK(enter_room), NULL);

	// Add leave room button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
	GtkWidget *leave_room_button = gtk_button_new_with_label ("Leave Room");
	gtk_table_attach_defaults(GTK_TABLE (table), leave_room_button, 4, 6, 7, 8); 
	gtk_widget_show (leave_room_button);
	g_signal_connect(leave_room_button, "clicked", G_CALLBACK(leave_room), NULL);

	GtkWidget * entryMessage = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryMessage), "New Message");
	gtk_table_attach_defaults(GTK_TABLE (table), entryMessage, 0, 6, 8, 9); 
	gtk_widget_show (entryMessage);

	// Add send button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
	GtkWidget *send_button = gtk_button_new_with_label ("Send");
	gtk_table_attach_defaults(GTK_TABLE (table), send_button, 0, 6, 9, 10); 
	const char * messageContent = gtk_entry_get_text(GTK_ENTRY(entryMessage));
	g_signal_connect_swapped(send_button, "clicked", G_CALLBACK(send_message), entryMessage);
	g_signal_connect(send_button, "clicked", G_CALLBACK(get_message), NULL);
	gtk_widget_show (send_button);

	GtkWidget * image = gtk_image_new_from_file("heixia.jpg");
	gtk_table_attach_defaults(GTK_TABLE (table), image, 6, 8, 0, 4); 
	gtk_widget_show (image);

	GtkWidget *label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), "<big>User Info</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), label, 6, 8, 4, 5); 
	gtk_widget_show (label);

	labelName = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelName), "<big>User Name</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelName, 6, 8, 5, 6); 
	gtk_widget_show (labelName);

	labelPass = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelPass), "<big>Password</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelPass, 6, 8, 6, 7); 
	gtk_widget_show (labelPass);

	GtkWidget * labelTime = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelTime), "<big>Current Time : </big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelTime, 6, 7, 7, 8); 
	gtk_widget_show (labelTime);

	GtkWidget * darea = gtk_drawing_area_new();
	gtk_table_attach_defaults(GTK_TABLE (table), darea, 7, 8, 7, 8); 
	g_signal_connect(darea, "expose-event", G_CALLBACK(on_expose_event), NULL);
	g_timeout_add(10, (GSourceFunc) time_handler, (gpointer) window);
	time_handler(window);


	GtkWidget *sign_up_button = gtk_button_new_with_label ("Sign Up / Log In");
	gtk_table_attach_defaults(GTK_TABLE (table), sign_up_button, 6, 8, 8, 9); 
	gtk_widget_show (sign_up_button);
	g_signal_connect(sign_up_button, "clicked", G_CALLBACK(sign_up), NULL);

	labelRoomNameEntered = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(labelRoomNameEntered), "<big>Room Entered</big>");
	gtk_table_attach_defaults(GTK_TABLE (table), labelRoomNameEntered, 6, 8, 8, 9); 
	gtk_widget_show (labelRoomNameEntered);

	GtkWidget *exit_button = gtk_button_new_with_label ("Exit IRC");
	gtk_table_attach_defaults(GTK_TABLE (table), exit_button, 6, 8, 9, 10); 
	gtk_widget_show (exit_button);
	g_signal_connect(exit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
	
	gtk_widget_show (darea);
	gtk_widget_show (table);
	gtk_widget_show (window);

	gtk_main ();
	
	return 0;
}
