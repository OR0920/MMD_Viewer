#include"GUI_Util.h"
#include"Error.h"

using namespace System;


MainWindow& MainWindow::Instance()
{
	static MainWindow inst;
	return inst;
}

Result MainWindow::Create(int width, int height)
{
	return Result::SUCCESS;
}

MainWindow::MainWindow()
{

}

MainWindow::~MainWindow()
{

}