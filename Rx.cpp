#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

// UDP通信用ヘッダ
// #include <sys/sock.h> // linux
#include <WinSock2.h>    // windows
#include <Windows.h>
#include <Windowsx.h>

// Windows.hをインクルードしたい場合は、Windows.ｈよりもWinsock2.hを先にインクルード
// なんならWindows.ｈだけでも、Winsock2.hだけでも動く

// UDP通信用ライブラリ
#pragma comment(lib, "ws2_32.lib")
// inet_addr()関数で警告が出る場合は以下で警告を無効化
#pragma warning(disable:4996)

// プロトタイプ宣言
void recv_coordinates();
void draw_line();
void show_window();

// ソケット通信winsockの立ち上げ
// wsaDataはエラー取得時に使用
WSAData wsaData;

// MAKEWORD(2, 0)はwinsockのバージョン2.0
int err = WSAStartup(MAKEWORD(2, 0), &wsaData);

// socket作成 
// socketを通してデータのやり取りを行う
// socket(アドレスファミリ, ソケットタイプ, プロトコル)
// AF_INETはIPv4、SOCK_DGRAMはUDP通信
// 0はプロトコルファミリーの種類ごとに一つのプロトコルのみをサポート
SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

// アドレス等を格納
struct sockaddr_in addr;

// バッファのサイズ
constexpr auto BUFF_SIZE = 8;

// グローバル座標
int old_x1 = 0, old_y1 = 0, new_x1 = 0, new_y1 = 0;
int old_x2 = 0, old_y2 = 0, new_x2 = 0, new_y2 = 0;

// 座標を格納するキュー
std::queue<int> qx, qy;

// 終了判定（終了:true, 続行:false）
bool is_ended = false;

// 線の色（初期状態は緑色）
cv::Scalar color = cv::Scalar(0, 255, 0);
// 線の太さ
int line_weight = 10;

// パソコンサイズの黒画面
cv::Mat img(cv::Size(1920, 1080), CV_8UC3, cv::Scalar(0, 0, 0));


int main()
{
    if (err != 0)
    {
        switch (err)
        {
        case WSASYSNOTREADY:
            std::cout << "WSASYSNOTREADY" << std::endl;
            break;
        case WSAVERNOTSUPPORTED:
            std::cout << "WSAVERNOTSUPPORTED" << std::endl;
            break;
        case WSAEINPROGRESS:
            std::cout << "WSAEINPROGRESS" << std::endl;
            break;
        case WSAEPROCLIM:
            std::cout << "WSAEPROCLIM" << std::endl;
            break;
        case WSAEFAULT:
            std::cout << "WSAEFAULT" << std::endl;
            break;
        }
    }

    if (sock == INVALID_SOCKET)
    {
        std::cout << "Socket failed" << std::endl;
    }

    addr.sin_family = AF_INET;     // IPv4
    addr.sin_port = htons(50008);  // 通信ポート番号設定
    addr.sin_addr.S_un.S_addr = INADDR_ANY;  // INADDR_ANYはすべてのアドレスからのパケットを受信

    // バインド
    // アドレス等の情報をsocketに登録
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));

    // ブロッキング、ノンブロッキングの設定
    //（任意、必須コードではない（ソケットの初期設定はブロッキングモードなため）
    // val = 0 : ブロッキングモード データが受信されるまで待機
    // val = 1 : ノンブロッキング データが受信されなくても次の処理へ
    // u_long val = 1;
    // ioctlsocket(sock, FIONBIO, &val);

    // タイムアウト時間の設定
    // struct timeval tv;
    // tv.tv_sec = 0;
    // tv.tv_usec = 10;
    // if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) < 0) 
    // {
    //     perror("Error");
    // }

    std::thread th_a(recv_coordinates);
    std::thread th_b(draw_line);
    std::thread th_c(show_window);

    th_a.join();
    th_b.join();
    th_c.join();

    // ウィンドウを閉じる
    cv::destroyAllWindows();

    // socketの破棄
    closesocket(sock);

    // すべてのスレッドのwinsockを終了
    WSACleanup();

    return 0;
}


// 座標を受信し、動的配列に格納
void recv_coordinates()
{
    while (!is_ended)
    {
        // 受信用のバッファ
        char old_xx[BUFF_SIZE];
        char old_yy[BUFF_SIZE];
        char new_xx[BUFF_SIZE];
        char new_yy[BUFF_SIZE];

        // データ受信
        // recv(ソケット, 受信データ, データのバイト数, フラグ);
        recv(sock, old_xx, BUFF_SIZE, 0);
        old_x1 = atoi(old_xx);

        // 送信側からのキーイベントを検知
        // 最初だけ確認すればよい。
        if (std::strcmp(old_xx, "quit") == 0)
        {
            is_ended = true;
            break;
        }
        else if (std::strcmp(old_xx, "reset") == 0)
        {
            cv::rectangle(img, cv::Point(0, 0), cv::Point(1920, 1080), cv::Scalar(0, 0, 0), cv::FILLED);
            continue;
        }
        else if (std::strcmp(old_xx, "black") == 0)
        {
            color = cv::Scalar(0, 0, 0);
            continue;
        }
        else if (std::strcmp(old_xx, "white") == 0)
        {
            color = cv::Scalar(255, 255, 255);
            continue;
        }
        else if (std::strcmp(old_xx, "blue") == 0)
        {
            color = cv::Scalar(255, 0, 0);
            continue;
        }
        else if (std::strcmp(old_xx, "green") == 0)
        {
            color = cv::Scalar(0, 255, 0);
            continue;
        }
        else if (std::strcmp(old_xx, "plus") == 0)
        {
            line_weight += 1;
            continue;
        }
        else if (std::strcmp(old_xx, "minus") == 0)
        {
            line_weight -= 1;
            continue;
        }

        recv(sock, old_yy, BUFF_SIZE, 0);
        old_y1 = atoi(old_yy);
        recv(sock, new_xx, BUFF_SIZE, 0);
        new_x1 = atoi(new_xx);
        recv(sock, new_yy, BUFF_SIZE, 0);
        new_y1 = atoi(new_yy);

        // キューにプッシュ
        qx.push(old_x1);
        qy.push(old_y1);
        qx.push(new_x1);
        qy.push(new_y1);
    }
}

// 動的配列に格納された座標を基に線を描写
void draw_line()
{    
    while (!is_ended)
    {
        if ((qx.size() >= 2) && (qy.size() >= 2))
        {
            old_x2 = qx.front();
            qx.pop();
            old_y2 = qy.front();
            qy.pop();
            new_x2 = qx.front();
            qx.pop();
            new_y2 = qy.front();
            qy.pop();

            // キューに格納された座標を基に線を描写
            cv::line(img, cv::Point(old_x2, old_y2), cv::Point(new_x2, new_y2), color, line_weight, cv::LINE_AA);
        }
    }
}

// ウィンドウを表示
void show_window()
{
    // 表示するウィンドウに名前を付ける
    cv::namedWindow("Rx", cv::WINDOW_NORMAL);
    // フルスクリーン表示
    // cv::setWindowProperty("Rx", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    while (!is_ended)
    {
        cv::imshow("Rx", img);
        cv::waitKey(1);
    }
}
