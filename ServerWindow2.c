#include <gtk/gtk.h>

GtkWidget *main_window; // 전역 변수로 메인 윈도우를 저장.
GtkWidget *button_total_inventory; // 전역 변수로 버튼 저장.
GtkWidget *button_inventory_history; // 전역 변수로 버튼 저장.
GtkWidget *selected_button = NULL; // 선택된 버튼을 저장하기 위한 전역 변수.

// 종료 버튼을 눌렀을 때 호출되는 콜백 함수.
void close_application(GtkWidget *widget, gpointer data) {
    gtk_main_quit(); // 애플리케이션 종료.
}

// 총 물류 현황 화면 생성 함수
GtkWidget* create_total_inventory_screen() {
    GtkWidget *total_inventory_box;
    GtkWidget *label;

    total_inventory_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    label = gtk_label_new("총 물류 현황 화면");

    gtk_box_pack_start(GTK_BOX(total_inventory_box), label, TRUE, TRUE, 0);

    return total_inventory_box;
}

// 물류 이력 관리 화면 생성 함수
GtkWidget* create_inventory_history_screen() {
    GtkWidget *inventory_history_box;
    GtkWidget *label;

    inventory_history_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    label = gtk_label_new("물류 이력 관리 화면");

    gtk_box_pack_start(GTK_BOX(inventory_history_box), label, TRUE, TRUE, 0);

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
    gtk_stack_set_visible_child_name(stack, name);
    
    // 선택된 버튼 업데이트
    selected_button = widget;
    update_button_style(button_total_inventory);
    update_button_style(button_inventory_history);
    update_button_style(widget);
}

// 버튼 생성 함수 (이미지와 텍스트를 포함)
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
    g_object_unref(pixbuf); // 원본 이미지를 해제
    image = gtk_image_new_from_pixbuf(scaled_pixbuf);
    g_object_unref(scaled_pixbuf); // 크기가 조절된 이미지를 해제

    label = gtk_label_new(label_text);

    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 5);

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

int main(int argc, char *argv[]) {
    GtkWidget *main_box;
    GtkWidget *button_box;
    GtkWidget *stack;
    GtkWidget *button_exit; // 종료 버튼 추가.
    GtkWidget *title_label; // 타이틀 레이블 추가

    gtk_init(&argc, &argv);

    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "물류 관리 시스템");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600); // 기본 창 크기 설정
    gtk_container_set_border_width(GTK_CONTAINER(main_window), 0);

    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

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

    gtk_widget_show_all(main_window);
    gtk_main();

    return 0;
}
