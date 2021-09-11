# include <Siv3D.hpp>
# include <windows.h>

void Main()
{
    //変数定義
    int process_counter = 140;
    int mouse_button_counter = 140;
    bool isRanning = false;
    int process_state = 1;      //処理の状態(0:開始,1:待機,2;終了)
    String gui_text[32];
    const Font font_size_15(15);

    //ウインドウ設定
    Window::Resize(600, 200);
    Window::SetTitle(U"Minecraft Trap Automation Tool");
    Graphics::SetTargetFrameRateHz(60); //fpsを60に固定

    //言語ファイルをロード--------------------------------------------------------------

    //国コード取得(変換 : WCHAR -> char -> String)

    WCHAR lang_code_buffer_wchar[256];
    char lang_code_buffer_char[256];
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SNAME, lang_code_buffer_wchar, _countof(lang_code_buffer_wchar));
    size_t ReturnValue;
    wcstombs_s(&ReturnValue, lang_code_buffer_char, 256, lang_code_buffer_wchar, 256);
    //国コード
    String lang_code = Unicode::FromUTF8(lang_code_buffer_char);

    //国コードを元にパスを合成
    String path = U".lang";
    path.insert(0, lang_code);
    path.insert(0, U"Language\\");

    //言語ファイルをオープンする
    TextReader reader;
    //行の内容を読み込む変数
    String line;
    //言語ファイルがない場合(英語表示)
    if (!reader.open(path))
    {
        reader.open(U"language\\en-US.lang");
        //英語ファイルが見つからない場合
        if (!reader)
        {
            throw Error(U"Language file not found");
        }
        //行数の表示用のカウント
        size_t i = 0;

        //1行ずつ読み込む
        while (reader.readLine(line) && i < 32)
        {
            gui_text[i] = line;
            i++;
        }
    }
    //言語ファイルがある場合
    else {
        //行数の表示用のカウント
        size_t i = 0;

        //1行ずつ読み込む
        while (reader.readLine(line) && i < 32)
        {
            gui_text[i] = line;
            i++;
        }
    }
    //言語ファイルをクローズ
    reader.close();

    //説明文合成
    gui_text[3].insert(0, U"\n");
    gui_text[3].insert(0, gui_text[2]);
    gui_text[3].insert(0, U"\n");
    gui_text[3].insert(0, gui_text[1]);
    gui_text[3].insert(0, U"\n");
    gui_text[3].insert(0, gui_text[0]);

    //各種情報表示
    Print(U"Minecraft Trap Automation Tool");
    Print(U"ver.1.0.0");
    Print(U"[Locale ID] = "+lang_code);
    Print(U"Launch Succeeded");

    //ループ---------------------------------------------------------------------------
    while (System::Update())
    {
        //GUI--------------------------------------------------------------------------
        //開始ボタン
        if (SimpleGUI::Button(U"Start", Vec2(300, 25)) && isRanning == false)
        {
            process_state = 2;
            Window::Minimize();
        }
        //終了ボタン
        if (SimpleGUI::Button(U"Stop ", Vec2(400, 25)) && isRanning == true)
        {
            process_state = 0;
            Window::Restore();
        }
        //説明表示
        font_size_15(gui_text[3]).draw(230, 100);

        //実行処理-------------------------------------------------------------------------
        if (process_counter < 151)process_counter++;
        //開始後一度目の処理は少しタイミングをずらす
        if (process_counter == 10 && isRanning == true) {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        }
        if (process_counter == 20 && isRanning == true) {
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
        //処理を繰り返す---------------------------------------------
        if (process_counter == 120 && isRanning == true) {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        }
        if (process_counter == 130 && isRanning == true) {
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
        if (process_counter == 150 && isRanning == true) {
            process_counter = 109;  //process_counterを減らしてループさせる
        }
        //------------------------------------------------------------

        //マウス中央ボタンを使った開始及び終了処理
        if (mouse_button_counter <= 60)mouse_button_counter++;  //前回のマウス中央ボタンの仕様から1秒経過したことを確認する

        if (GetAsyncKeyState(VK_MBUTTON) && isRanning == true && mouse_button_counter > 60) {
            process_state = 0;
            mouse_button_counter = 0;
        }

        if (GetAsyncKeyState(VK_MBUTTON) && isRanning == false && mouse_button_counter > 60) {
            process_state = 2;
            mouse_button_counter = 0;
            process_counter = 0;
        }
        
        //状態管理
        //開始
        if (process_state == 0) {
            isRanning = true;
            Print(U"Stop");
            Window::SetTitle(U"Minecraft Trap Automation Tool");
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            process_state = 1;
        }
        //待機
        if (process_state == 1) {

        }
        //終了
        if (process_state == 2) {
            isRanning = true;
            Print(U"Start");
            Window::SetTitle(U"(Running) Minecraft Trap Automation Tool");
            process_counter = 0;
            process_state = 1;
        }
    }
}