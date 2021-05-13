//---------------------------------------------------------------------------

#ifndef UFProbeTestH
#define UFProbeTestH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
 #include <System.StrUtils.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include "Templates.h"
#include "Probe.h"
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>
#include "USignals.h"
#include "UAnalizator.h"
#include <Vcl.Dialogs.hpp>
#include "UPsu.h"
#include "UCalibrate.h"
#include "UMgqs.h"
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TButton *Button1;
	TSplitter *Splitter1;
	TPanel *Panel3;
	TListBox *Box1;
	TPanel *Panel2;
	TPanel *Panel4;
	TButton *Button2;
	TButton *Button3;
	TPanel *Panel5;
	TPopupMenu *Popup3;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *N4;
	TImageList *IList1;
	TToolBar *ToolBar1;
	TToolButton *ToolButton1;
	TToolButton *ToolButton5;
	TToolButton *ToolButton4;
	TToolButton *ToolButton6;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TToolButton *ToolButton7;
	TToolButton *ToolButton8;
	TStatusBar *StatusBar1;
	TSaveDialog *SaveDlg;
	TOpenDialog *OpenDlg;
	TPanel *Panel6;
	TPanel *Panel7;
	TPageControl *PageControl1;
	TSplitter *Splitter2;
	TTabSheet *TabSheet1;
	TListBox *ListBox2;
	TLabel *Label3;
	TEdit *Edit2;
	TUpDown *UpDown3;
	TUpDown *UpDown2;
	TEdit *Edit1;
	TLabel *Label2;
	TCheckBox *CheckBox1;
	TButton *Button4;
	TLabel *Label1;
	TLabel *Label4;
	TEdit *Edit3;
	TPanel *Panel8;
	TLabel *Label5;
	TEdit *Edit4;
	TButton *Button5;
	TTabSheet *TabSheet2;
	TRichEdit *Memo2;
	TSplitter *Splitter3;
	TRichEdit *Memo1;
	TButton *Button6;
	TButton *Button7;
	TMemo *Memo_out;
	TComboBox *ComboBox1;
	TButton *Button8;
	TCheckBox *CheckBox2;
	TMemo *Memo3;
	TLabel *Label6;
	TLabel *Label7;
	TEdit *Edit5;
	TEdit *Edit6;
	TButton *Button9;
	TEdit *Edit7;
	TLabel *Label8;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall ToolButton1Click(TObject *Sender);
	void __fastcall N1Click(TObject *Sender);
	void __fastcall N2Click(TObject *Sender);
	void __fastcall N3Click(TObject *Sender);
	void __fastcall N4Click(TObject *Sender);
	void __fastcall ToolButton2Click(TObject *Sender);
	void __fastcall ToolButton3Click(TObject *Sender);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall Edit1Change(TObject *Sender);
	void __fastcall UpDown2ChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall UpDown3ChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall ToolButton4Click(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
	void __fastcall Button8Click(TObject *Sender);
	void __fastcall CheckBox2Click(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall Button9Click(TObject *Sender);
private:	// User declarations

	Probe 				_probe;
	USlots				_slots;

	u32		_low;
	u32		_hi;

	void	__fastcall OnLogChange(String data);
	void	__fastcall OnProbeAccess(String data);
	void	__fastcall OnProbeError(String data);

	HList			get_COM_ports();
	void	__fastcall show_ports();
	String	__fastcall	getDeviceId();

	void	__fastcall 	connect();
	void	__fastcall	disconnect();
	void	__fastcall	connect_psu();
	void	__fastcall	disconnect_psu();

	void	__fastcall	execute();

	void	__fastcall	save(String filename);
	void	__fastcall	load(String filename);

	String	__fastcall edit_control(String text);

	void	__fastcall get_data();
	void	__fastcall set_data();
	void	__fastcall accept();
	void	__fastcall set_low(i32 low);
	void	__fastcall set_hi(i32 hi);
	void	__fastcall scroll(i32 offset);

	void	__fastcall send_count();
	void	__fastcall send_data();

	vector<pair<i32, i32> >	__fastcall load_vect(u8 mode, bool cut);
	void	__fastcall show_vect(vector<pair<i32, i32> > vect);
	vector<pair<i32, i32> >		_vect;
public:		// User declarations
	__fastcall TForm3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;
//---------------------------------------------------------------------------
#endif
