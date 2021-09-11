//
// Siv3D August 2016 v2 for Visual Studio 2019
// 
// Requirements
// - Visual Studio 2015 (v140) toolset
// - Windows 10 SDK (10.0.17763.0)
//

#include <Siv3D.hpp>
#include "Func.h"

void Main()
{
	//ウインドウ設定
	Window::SetTitle(L"Launcher");
	Window::SetStyle(WindowStyle::Sizeable);

	//変数定義
	bool isRanning = true;		//ソフトウェア実行フラグ
	int device_ID = 0;			//録音デバイスのID
	int window_hide_flag = 0;	//ウインドウ保持関連設定
	double threshold = 0;		//しきい値
	String device_name;			//録音デバイスの名前
	String lang_code;			//国コード
	String gui_text[32];		//guiに表示する文字列

	//設定情報をファイルから取得--------------------------------------------------------
	TextReader reader;
	reader.open(L"Fishing\\DeviceID.txt");	//ファイルをオープン
	String line;							//読み込んだ行を一時保存する変数
	reader.readLine(line);					//1行目を読み込み
	device_ID = Parse<int>(line);			//デバイスIDを取得
	reader.readLine(line);					//2行目を読み込み
	threshold = Parse<double>(line);		//しきい値を取得
	reader.readLine(line);					//3行目を読み込み
	device_name = Parse<String>(line);		//デバイス取得
	reader.readLine(line);					//4行目を読み込み
	window_hide_flag = Parse<int>(line);	//ウインドウ保持関連設定を取得
	reader.close();							//ファイルをクローズ

	//多言語対応------------------------------------------------------------------------

	//国コード取得(変換 : char -> String)
	char lang_code_buffer_char[256];
	GetLocaleInfoChar(lang_code_buffer_char);
	lang_code = Widen(lang_code_buffer_char);

	//言語ファイルのパスを合成
	String path = L".lang";
	path.insert(0, lang_code);
	path.insert(0, L"Language\\");

	//言語ファイルがない場合(英語表示)
	if (!reader.open(path))
	{
		reader.open(L"language\\en-US.lang");
		//オープンに失敗
		if (!reader)
		{
			Println(L"Error:Language file not found");
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

	//録音デバイス一覧を取得------------------------------------------------------

	//デバイス一覧配列
	Array<String> Buffer(64);
	int a = 0;	//使用した配列の個数
	for (const auto& recordercheck : RecorderManager::Enumerate())
	{
		//デバイスIDと録音デバイスの名前をBufferに格納
		Buffer[a] = Format(recordercheck.name);
		a++;
	}
	//配列の個数をデバイスの個数にして代入
	Array<String> Devices(a);
	for (int n = 0; n < a; n++) {
		Devices[n] = Buffer[n];
	}

	//gui設定----------------------------------------------------------------------

	Graphics::SetBackground(Color(220, 220, 220));
	GUIStyle style = GUIStyle::Default;
	style.borderRadius = 0;
	GUI gui(style);
	
	gui.setTitle(gui_text[0]);
	gui.addln(GUIText::Create(gui_text[1]));
	gui.addln(L"Devices", GUIRadioButton::Create(Devices));

	gui.addln(GUIText::Create(gui_text[2]));
	gui.addln(L"port", GUITextField::Create(6));
	gui.textField(L"port").setText(Format(threshold));

	gui.addln(GUIText::Create(gui_text[3]));
	gui.addln(L"Fishing", GUIButton::Create(L"Minecraft Auto Fishing Tool"));
	gui.addln(L"Trap", GUIButton::Create(L"Minecraft Trap Automation Tool"));

	gui.setCenter(Window::Center());

	gui.radioButton(L"Devices").check(device_ID);

	//ループ------------------------------------------------------------------------

	while (System::Update() && isRanning == true)
	{
		//ラジオボタン
		for (int n = 0; n < a; n++) {
			if (gui.radioButton(L"Devices").checked(n))
			{
				device_ID = n;
			}
		}

		//Mincraft Auto Fishing Tool起動
		if (gui.button(L"Fishing").pushed)
		{
			gui.hide();

			TextWriter writer(L"Fishing\\DeviceID.txt");
			writer.writeln(Format(device_ID));				//デバイスID
			writer.writeln(gui.textField(L"port").text);	//しきい値
			writer.writeln(Devices[device_ID]);				//デバイス名
			writer.writeln(Format(window_hide_flag));		//ウインドウ設定
			writer.close();

			isRanning = false;
			System::CreateProcess(L"Fishing\\Minecraft Auto Fishing Tool.exe");
		}
		//Minecraft Trap Automation Tool起動
		else if (gui.button(L"Trap").pushed)
		{
			gui.hide();

			TextWriter writer(L"Fishing\\DeviceID.txt");
			writer.writeln(Format(device_ID));				//デバイスID
			writer.writeln(gui.textField(L"port").text);	//しきい値
			writer.writeln(Devices[device_ID]);				//デバイス名
			writer.writeln(Format(window_hide_flag));		//ウインドウ設定
			writer.close();

			isRanning = false;
			System::CreateProcess(L"Trap\\Minecraft Trap Automation Tool.exe");
		}				
	}
	//-------------------------------------------------------------------------------
}
