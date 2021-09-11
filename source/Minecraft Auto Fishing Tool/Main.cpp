# include <Siv3D.hpp>
# include <windows.h>

void Save(bool window_close_flag, int device_id, double threshold, String device_name);

void Main()
{
    //変数定義
    double threshold = 0.05;         //しきい値
    int device_ID = 0;               //Launcherで取得した録音デバイスのID
    String device_name;              //Launcherで取得した録音デバイスの名前
    bool window_hide_flag = true;    //釣り実行時にウインドウを隠すフラグ
    String gui_text[32];             //guiに表示する文字列
    bool isRanning = false;          //釣り実行中フラグ
    int fishing_state = 1;           //釣り実行状態
    double max_amplitude = 0;        //最大振幅
    int fishing_counter = 140;       //釣り制御用カウンター
    int mouse_button_counter = 140;  //マウスボタン制御用カウンター
    const Font font_size_30(30);
    const Font font_size_15(15);

    //ウインドウ設定
    Window::Resize(800, 500);
    Window::SetTitle(U"Minecraft Auto Fishing Tool");

    //設定読み込み---------------------------------------------------------------------
    //ファイルをオープンする
    TextReader reader(U"DeviceID.txt");

    //オープンに失敗
    if (!reader)
    {
        throw Error(U"Configuration file not found");
    }

    //行の内容を読み込む変数
    String line;

    //読み込み
    reader.readLine(line);				//一行目を読み込み
    device_ID = Parse<int>(line);		
    reader.readLine(line);				//二行目を読み込み
    threshold = Parse<double>(line);	
    reader.readLine(line);				//三行目を読み込み
    device_name = line;		            
    reader.readLine(line);				//四行目を読み込み
    if (line == U"0") {
        window_hide_flag = false;
    }
    else {
        window_hide_flag = true;
    }
    //ファイルをクローズ
    reader.close();

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
    // ファイルをクローズ
    reader.close();

    //説明文合成
    gui_text[10].insert(0, U"\n");
    gui_text[10].insert(0, gui_text[9]);
    gui_text[10].insert(0, U"\n");
    gui_text[10].insert(0, gui_text[8]);
    gui_text[10].insert(0, U"\n");
    gui_text[10].insert(0, gui_text[7]);
    gui_text[10].insert(0, U"\n");
    gui_text[10].insert(0, gui_text[6]);

    //初期化----------------------------------------------------------------------------
    //録音デバイスをセットアップ
    Microphone mic(device_ID);

    if (!mic)
    {
        //録音デバイス利用できない場合
        throw Error(U"Microphone not available");
    }

    //録音デバイススタート
    mic.start();
    FFTResult fft;

    //各種情報表示
    Print(U"App ver.1.0.0");
    Print(U"Sound device loaded");
    Print(U"[Device name]");
    Print(device_name);
    Print(U"[Locale ID]");
    Print(lang_code);
    Print(U"Launch Succeeded");

    //ループ---------------------------------------------------------------------------
    while (System::Update())
    {
        //高速フーリエ変換-------------------------------------------------------------
        //FFTの結果を取得
        mic.fft(fft);

        //最大振幅リセット
        max_amplitude = 0;

        //結果を変形    
        for (auto i : step(800))
        {
            const double size = Pow(fft.buffer[i], 0.5f) * 1000;
            if (max_amplitude < size) {
                max_amplitude = size;
            }
        }

        //GUI--------------------------------------------------------------------------
        SimpleGUI::Slider(threshold, Vec2(500, 255), 300, true);
        //釣り開始ボタン
        if (SimpleGUI::Button(gui_text[0], Vec2(300, 20)) && isRanning == false)
        {
            fishing_state = 2;
        }
        //釣り終了ボタン
        if (SimpleGUI::Button(gui_text[1], Vec2(500, 20)) && isRanning == true)
        {
            fishing_state = 0;
        }
        //釣り開始時にウインドウを閉じるか設定するチェックボックス
        SimpleGUI::CheckBox(window_hide_flag, gui_text[2], Vec2(300, 65));
        //設定保存ボタン
        if (SimpleGUI::Button(gui_text[3], Vec2(500, 340)))
        {
            Save(window_hide_flag,device_ID,threshold,device_name);
        }
        //最大振幅を表示
        font_size_30(gui_text[4], max_amplitude / 1000).draw(200, 150);
        RectF(200, 200, (max_amplitude / 1000) * (Window::ClientWidth() - 200), 30).draw();
        //しきい値を表示
        font_size_30(gui_text[5], threshold).draw(200, 250);
        RectF(200, 300, threshold * (Window::ClientWidth() - 200), 30).draw();
        //操作方法を表示
        font_size_15(gui_text[10]).draw(200, 350);

        //釣り実行処理------------------------------------------------------------------
        if (fishing_counter <= 140)fishing_counter++;

        //fishing_counterが140より大きく、音量がしきい値より高い場合に釣り竿巻き上げ
        if (threshold < (max_amplitude / 1000) && fishing_counter > 140 && isRanning == true) {
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
            fishing_counter = 0;
        }
        if (fishing_counter == 10 && isRanning == true)mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);

        //再度釣り開始
        if (fishing_counter == 70 && isRanning == true)mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
        if (fishing_counter == 80 && isRanning == true)mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);


        //マウス中央ボタンによる釣り終了処理
        if (mouse_button_counter <= 60)mouse_button_counter++;
        //実行中かつマウス中央ボタンが押された場合（mouse_button_counterで前回の実行から1秒経過しているか確認）
        if (GetAsyncKeyState(VK_MBUTTON) && isRanning == true && mouse_button_counter > 60) {
            fishing_state = 0;
            mouse_button_counter = 0;
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
        }

        //釣り終了(fishing_state==0) 
        if (fishing_state == 0) {
            isRanning = false;
            if (window_hide_flag == true) {
                Window::Restore();
            }
            fishing_state = 1;
            Print(U"Stop Fishing");
            Window::SetTitle(U"Minecraft Auto Fishing Tool");
        }
        //待機状態(fishing_state==1)
        if (fishing_state == 1) {

        }
        //釣り開始(fishing_state==2)
        if (fishing_state == 2) {
            isRanning = true;
            if (window_hide_flag == true) {
                Window::Minimize();
            }
            Print(U"Start Fishing");
            Window::SetTitle(U"(Running) Minecraft Auto Fishing Tool");
            fishing_state = 1;
        }

    }
    //--------------------------------------------------------------------------------------
    Print(U"Application closed");
}

void Save(bool window_close_flag, int device_id, double threshold, String device_name) {
    //ファイルオープン
    TextWriter writer;

    //オープンに失敗
    if (!writer.open(U"DeviceID.txt"))
    {
        throw Error(U"Error");
    }
    writer << device_id;
    writer << threshold;
    writer << device_name;
    if (window_close_flag == true) {
        writer << 1;
    }
    else {
        writer << 0;
    }

    //ファイルクローズ
    writer.close();

    Print(U"Setting was saved");
}