#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

GtkWidget *main_window; // 전역 변수로 메인 윈도우를 저장.
GtkWidget *button_total_inventory; // 전역 변수로 버튼 저장.
GtkWidget *button_inventory_history; // 전역 변수로 버튼 저장.
GtkWidget *selected_button = NULL; // 선택된 버튼을 저장하기 위한 전역 변수.
//GtkWidget *entry_product_name; // 상품명 입력란.
//GtkWidget *entry_product_code; // 상품코드 입력란.
GtkWidget *treeview; // 조회 결과를 표시할 트리뷰.

GtkWidget *t_entry_product_name; // 총 물류현황 상품명 입력란.
GtkWidget *t_entry_product_code; // 총 뮬류현황 상품코드 입력란.

GtkWidget *h_entry_product_name; // 물류 이력관리 상품명 입력란.
GtkWidget *h_entry_product_code; // 물류 이력관리 상품코드 입력란.

GtkWidget *total_treeview;
GtkWidget *history_treeview;


struct Product {
    const char *code;
    const char *name;
    int quantity;
} products[] = {
    {"12345", "Product A", 100},
    {"67890", "Product B", 50},
    {"54321", "Product C", 30}
};

void updateProductQuantity(const char *name, int quantityToAdd);
void parseMessageAndUpdate(char *message);

// 종료 버튼을 눌렀을 때 호출되는 콜백 함수.
void close_application(GtkWidget *widget, gpointer data) {
    gtk_main_quit(); // 애플리케이션 종료.
}

// 총 물류 현황 조회 함수
void search_inventory(GtkWidget *widget, gpointer data) {
    const gchar *product_name = gtk_entry_get_text(GTK_ENTRY(t_entry_product_name));
    const gchar *product_code = gtk_entry_get_text(GTK_ENTRY(t_entry_product_code));

    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(total_treeview))); // total_treeview 변수 사용
    gtk_list_store_clear(store);

    for (int i = 0; i < sizeof(products)/sizeof(products[0]); i++) {
        if ((strlen(product_name) == 0 || g_strcmp0(products[i].name, product_name) == 0) &&
            (strlen(product_code) == 0 || g_strcmp0(products[i].code, product_code) == 0)) {
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, products[i].code, 1, products[i].name, 2, products[i].quantity, -1);
        }
    }

    // 상품코드 기준으로 정렬
    GtkTreeSortable *sortable = GTK_TREE_SORTABLE(store);
    gtk_tree_sortable_set_sort_column_id(sortable, 0, GTK_SORT_ASCENDING);
}


//수량 업데이트
void updateProductQuantity(const char *name, int quantityToAdd) {
    int i;
    int size = sizeof(products) / sizeof(products[0]);

    for (i = 0; i < size; i++) {
        if (strcmp(products[i].name, name) == 0) {
            products[i].quantity += quantityToAdd;
            printf("Updated quantity of %s: %d\n", products[i].name, products[i].quantity);
            return;
        }
    }

    printf("Product not found.\n");
}


//메세지 파싱
void parseMessageAndUpdate(char *message) {
    char *name = strtok(message, " "); // 공백을 구분자로 사용
    char *quantityStr = strtok(NULL, " ");

    if (name != NULL && quantityStr != NULL) {
        int quantityToAdd = atoi(quantityStr);
        updateProductQuantity(name, quantityToAdd);
    } else {
        printf("Invalid message format.\n");
    }
}



// 총 물류 현황 화면 생성 함수
GtkWidget* create_total_inventory_screen() {
    GtkWidget *total_inventory_box;
    GtkWidget *search_box;
    GtkWidget *label_product_name;
    GtkWidget *label_product_code;
    GtkWidget *button_search;
    GtkWidget *scrolled_window;
    GtkListStore *store;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    //GtkWidget *total_treeview;

    total_inventory_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    search_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    label_product_name = gtk_label_new("상품명:");
    gtk_box_pack_start(GTK_BOX(search_box), label_product_name, FALSE, FALSE, 10);

    t_entry_product_name = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(search_box), t_entry_product_name, TRUE, TRUE, 0);

    label_product_code = gtk_label_new("상품코드:");
    gtk_box_pack_start(GTK_BOX(search_box), label_product_code, FALSE, FALSE, 5);

    t_entry_product_code = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(search_box), t_entry_product_code, TRUE, TRUE, 0);

    button_search = gtk_button_new_with_label("조회");
    g_signal_connect(button_search, "clicked", G_CALLBACK(search_inventory), NULL);
    gtk_box_pack_start(GTK_BOX(search_box), button_search, FALSE, FALSE, 10);

    gtk_box_pack_start(GTK_BOX(total_inventory_box), search_box, FALSE, FALSE, 15);

    // 트리뷰를 생성 후 조회 결과 표시
    store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    total_treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("상품코드", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(total_treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("상품명", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(total_treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("상품개수", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(total_treeview), column);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), total_treeview);
    gtk_box_pack_start(GTK_BOX(total_inventory_box), scrolled_window, TRUE, TRUE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 15);

    return total_inventory_box;
}

// 물류 이력 조회 함수
void search_inventory_history(GtkWidget *widget, gpointer data) {
    const gchar *product_name = gtk_entry_get_text(GTK_ENTRY(h_entry_product_name));
    const gchar *product_code = gtk_entry_get_text(GTK_ENTRY(h_entry_product_code));

    // 예시 데이터
    struct InventoryHistory {
        const gchar *code;
        const gchar *name;
        int quantity;
        const gchar *history;
    } inventory_history[] = {
        {"12345", "상품 A", 10, "입고"},
        {"67890", "상품 B", 5, "입고"},
        {"54321", "상품 C", 3, "입고"}
    };

    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(history_treeview))); // history_treeview 변수 사용
    gtk_list_store_clear(store);

    for (int i = 0; i < sizeof(inventory_history)/sizeof(inventory_history[0]); i++) {
        if ((strlen(product_name) == 0 || g_strcmp0(inventory_history[i].name, product_name) == 0) &&
            (strlen(product_code) == 0 || g_strcmp0(inventory_history[i].code, product_code) == 0)) {
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, inventory_history[i].code, 1, inventory_history[i].name, 2, inventory_history[i].quantity, 3, inventory_history[i].history, -1);
        }
    }
}

// 물류 이력 관리 화면 생성 함수
GtkWidget* create_inventory_history_screen() {
    GtkWidget *inventory_history_box;
    GtkWidget *search_box;
    GtkWidget *label_product_name;
    GtkWidget *label_product_code;
    GtkWidget *button_search;
    GtkWidget *scrolled_window;
    GtkListStore *store;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    //GtkWidget *history_treeview;

    inventory_history_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    search_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    label_product_name = gtk_label_new("상품명:");
    gtk_box_pack_start(GTK_BOX(search_box), label_product_name, FALSE, FALSE, 10);

    h_entry_product_name = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(search_box), h_entry_product_name, TRUE, TRUE, 0);

    label_product_code = gtk_label_new("상품코드:");
    gtk_box_pack_start(GTK_BOX(search_box), label_product_code, FALSE, FALSE, 5);

    h_entry_product_code = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(search_box), h_entry_product_code, TRUE, TRUE, 0);

    button_search = gtk_button_new_with_label("조회");
    g_signal_connect(button_search, "clicked", G_CALLBACK(search_inventory_history), NULL);
    gtk_box_pack_start(GTK_BOX(search_box), button_search, FALSE, FALSE, 10);

    gtk_box_pack_start(GTK_BOX(inventory_history_box), search_box, FALSE, FALSE, 15);

    // 트리뷰 생성 후 조회 결과 표시
    store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);
    history_treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("상품코드", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(history_treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("상품명", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(history_treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("상품개수", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(history_treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("이력", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(history_treeview), column);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), history_treeview);
    gtk_box_pack_start(GTK_BOX(inventory_history_box), scrolled_window, TRUE, TRUE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 15);

    return inventory_history_box;
}

// 버튼 스타일 업데이트 함수
void update_button_style(GtkWidget *button) {
    GtkStyleContext *context = gtk_widget_get_style_context(button);
    if (selected_button == button) {
        gtk_style_context_remove_class(context, "custom-button");
        gtk_style_context_add_class(context, "selected-button");
    } else {
        gtk_style_context_remove_class(context, "selected-button");
        gtk_style_context_add_class(context, "custom-button");
    }
}

// 버튼 클릭 시 호출되는 콜백 함수
void switch_to_stack_child(GtkWidget *widget, gpointer data) {
    GtkStack *stack = GTK_STACK(data);
    const gchar *name = gtk_widget_get_name(widget);

    // 선택된 버튼 업데이트
    selected_button = widget;
    update_button_style(button_total_inventory);
    update_button_style(button_inventory_history);
    update_button_style(widget);

    // 버튼에 따라 다른 화면 표시
    if (strcmp(name, "total_inventory") == 0) {
        gtk_stack_set_visible_child_name(stack, "total_inventory");
    } else if (strcmp(name, "inventory_history") == 0) {
        gtk_stack_set_visible_child_name(stack, "inventory_history");
    }
}

// 버튼 생성 함수 (이미지, 텍스트 포함)
GtkWidget* create_button_with_image(const gchar *image_path, const gchar *label_text, const gchar *button_name, GCallback callback, gpointer user_data) {
    GtkWidget *button;
    GtkWidget *box;
    GtkWidget *image;
    GtkWidget *label;

    button = gtk_button_new();
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(button), box);

    // 이미지 크기 설정
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 24, 24, GDK_INTERP_BILINEAR);
    g_object_unref(pixbuf); // 원본 이미지 해제
    image = gtk_image_new_from_pixbuf(scaled_pixbuf);
    g_object_unref(scaled_pixbuf); // 크기 조절된 이미지 해제

    label = gtk_label_new(label_text);

    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

    g_signal_connect(button, "clicked", callback, user_data);
    gtk_widget_set_name(button, button_name);
    gtk_style_context_add_class(gtk_widget_get_style_context(button), "custom-button"); // 버튼에 CSS 클래스 추가
    // 패딩 설정
    gtk_container_set_border_width(GTK_CONTAINER(button), 10);

    // CSS 스타일을 적용하여 버튼의 모양 및 패딩 변경
    GtkCssProvider *css_provider_button = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider_button, ".custom-button { font-family: 'NanumSquareRound'; background: #00006F; border-radius: 10px; padding: 15px; color: #FFFFFF; border-color: #484848; font-weight: 900; } .selected-button { font-family: 'NanumSquareRound'; background: #5872A5; border-radius: 10px; padding: 15px; color: #FFFFFF; border-color: #484848; font-weight: 900; }", -1, NULL);
    GtkStyleContext *context_button = gtk_widget_get_style_context(button);
    gtk_style_context_add_provider(context_button, GTK_STYLE_PROVIDER(css_provider_button), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider_button); // CSS 공급자 해제

    return button;
}

int server_socket;

void *socket_thread_func(void *arg) {
    int new_socket;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    // 서버 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 소켓 주소 구조체 초기화
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // 소켓을 포트에 바인딩
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // 클라이언트의 연결 요청을 대기하는 대기 큐의 크기 설정
    if (listen(server_socket, 10) == 0) {
        printf("Listening....\n");
    } else {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(new_addr);
    // 클라이언트와의 연결 수락
    new_socket = accept(server_socket, (struct sockaddr*)&new_addr, &addr_size);
    if (new_socket < 0) {
        perror("Acceptance failed");
        exit(EXIT_FAILURE);
    }

   // 클라이언트로부터 메시지 수신
    while (1) {
        recv(new_socket, buffer, BUFFER_SIZE, 0);
        printf("Client: %s\n", buffer);
        parseMessageAndUpdate(buffer);
        memset(buffer, 0, sizeof(buffer));
    }

    close(new_socket);
    close(server_socket);
    return NULL;
}


int main(int argc, char *argv[]) {
    GtkWidget *main_box;
    GtkWidget *button_box;
    GtkWidget *stack;
    GtkWidget *button_exit; // 종료 버튼 추가.
    GtkWidget *title_label; // 타이틀 레이블 추가

    gtk_init(&argc, &argv);

    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "물류 관리 시스템");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);
    g_signal_connect(main_window, "destroy", G_CALLBACK(close_application), NULL);

    main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(main_window), main_box);

    button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_name(button_box, "sidebar");
    gtk_box_set_homogeneous(GTK_BOX(button_box), FALSE);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);

    stack = gtk_stack_new();
    gtk_widget_set_name(stack, "content");
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 500);
    gtk_box_pack_start(GTK_BOX(main_box), stack, TRUE, TRUE, 0);

    GtkWidget *total_inventory_screen = create_total_inventory_screen();
    GtkWidget *inventory_history_screen = create_inventory_history_screen();

    gtk_stack_add_titled(GTK_STACK(stack), total_inventory_screen, "total_inventory", "총 물류 현황");
    gtk_stack_add_titled(GTK_STACK(stack), inventory_history_screen, "inventory_history", "물류 이력 관리");

    // CSS 스타일을 적용하여 화면에 배경색 설정
    GtkCssProvider *css_provider_ih = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider_ih, "#content { background: #E7E7E7; }", -1, NULL);
    GtkStyleContext *context_ih = gtk_widget_get_style_context(stack);
    gtk_style_context_add_provider(context_ih, GTK_STYLE_PROVIDER(css_provider_ih), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider_ih); // CSS 공급자 해제

    // 타이틀 레이블 생성 및 추가
    title_label = gtk_label_new("물류 관리\n  시스템");
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, "label { font-family: 'NanumSquareRound'; font-size: 18pt; color: #FFFFFF; padding: 15px; padding-top: 30px; font-weight: 900; }", -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(title_label);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider); // CSS 공급자를 해제

    // CSS 스타일을 적용하여 버튼 박스에 배경색 설정
    GtkCssProvider *css_provider_sidebar = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider_sidebar, "#sidebar { background: #00004B; }", -1, NULL);
    GtkStyleContext *context_sidebar = gtk_widget_get_style_context(button_box);
    gtk_style_context_add_provider(context_sidebar, GTK_STYLE_PROVIDER(css_provider_sidebar), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider_sidebar); // CSS 공급자 해제

    gtk_box_pack_start(GTK_BOX(button_box), title_label, FALSE, FALSE, 0);

    // 버튼 생성
    button_total_inventory = create_button_with_image("/home/heeya/Desktop/PR/images/warehouse.png", "총 물류 현황", "total_inventory", G_CALLBACK(switch_to_stack_child), stack);
    gtk_box_pack_start(GTK_BOX(button_box), button_total_inventory, FALSE, FALSE, 0);

    button_inventory_history = create_button_with_image("/home/heeya/Desktop/PR/images/list.png", "물류 이력 관리", "inventory_history", G_CALLBACK(switch_to_stack_child), stack);
    gtk_box_pack_start(GTK_BOX(button_box), button_inventory_history, FALSE, FALSE, 0);

    // 종료 버튼 생성
    button_exit = create_button_with_image("/home/heeya/Desktop/PR/images/x_icon.png", "종료", "exit", G_CALLBACK(close_application), NULL);
    gtk_box_pack_end(GTK_BOX(button_box), button_exit, FALSE, FALSE, 0);

    // 초기 상태 설정
    selected_button = button_total_inventory;
    update_button_style(button_total_inventory);
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "total_inventory");


    pthread_t thread_id;
    pthread_create(&thread_id, NULL, socket_thread_func, NULL);


    gtk_widget_show_all(main_window);
    gtk_main();

    return 0;
}
