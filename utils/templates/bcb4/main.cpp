//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <irsstd.h>
#include <irsdefs.h>

#include "main.h"
#include "config.h"
#include "mainprg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;

// Автоподключение mainprg
mpc_builder_t *mpc_builder = IRS_NULL;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  BuilderConsole = Console->Lines;
  mpc_builder = new mpc_builder_t();
  mpc_builder->Connect(this, init, deinit, tick, stop, stopped);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
  if (mpc_builder) delete mpc_builder;
  mpc_builder = IRS_NULL;
}
//---------------------------------------------------------------------------
