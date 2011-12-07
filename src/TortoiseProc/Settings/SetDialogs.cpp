// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2011 - TortoiseGit
// Copyright (C) 2003-2008 - TortoiseSVN

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "stdafx.h"
#include "TortoiseProc.h"
#include "SetMainPage.h"
#include "AppUtils.h"
#include "GITProgressDlg.h"
#include ".\setdialogs.h"

IMPLEMENT_DYNAMIC(CSetDialogs, ISettingsPropPage)
CSetDialogs::CSetDialogs()
	: ISettingsPropPage(CSetDialogs::IDD)
	, m_bShortDateFormat(FALSE)
	, m_bRelativeTimes(FALSE)
	, m_dwFontSize(0)
	, m_sFontName(_T(""))
	, m_bDiffByDoubleClick(FALSE)
	, m_bUseSystemLocaleForDates(FALSE)
	, m_bUseRecycleBin(TRUE)
{
	m_regAutoClose = CRegDWORD(_T("Software\\TortoiseGit\\AutoClose"));
	m_regShortDateFormat = CRegDWORD(_T("Software\\TortoiseGit\\LogDateFormat"), TRUE);
	m_regRelativeTimes = CRegDWORD(_T("Software\\TortoiseGit\\RelativeTimes"), FALSE);
	m_regUseSystemLocaleForDates = CRegDWORD(_T("Software\\TortoiseGit\\UseSystemLocaleForDates"), TRUE);
	m_regFontName = CRegString(_T("Software\\TortoiseGit\\LogFontName"), _T("Courier New"));
	m_regFontSize = CRegDWORD(_T("Software\\TortoiseGit\\LogFontSize"), 8);
	m_regDiffByDoubleClick = CRegDWORD(_T("Software\\TortoiseGit\\DiffByDoubleClickInLog"), FALSE);
	m_regUseRecycleBin = CRegDWORD(_T("Software\\TortoiseGit\\RevertWithRecycleBin"), TRUE);
}

CSetDialogs::~CSetDialogs()
{
}

void CSetDialogs::DoDataExchange(CDataExchange* pDX)
{
	ISettingsPropPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FONTSIZES, m_cFontSizes);
	m_dwFontSize = (DWORD)m_cFontSizes.GetItemData(m_cFontSizes.GetCurSel());
	if ((m_dwFontSize==0)||(m_dwFontSize == -1))
	{
		CString t;
		m_cFontSizes.GetWindowText(t);
		m_dwFontSize = _ttoi(t);
	}
	DDX_Control(pDX, IDC_FONTNAMES, m_cFontNames);
	DDX_Check(pDX, IDC_SHORTDATEFORMAT, m_bShortDateFormat);
	DDX_Check(pDX, IDC_RELATIVETIMES, m_bRelativeTimes);
	DDX_Control(pDX, IDC_AUTOCLOSECOMBO, m_cAutoClose);
	DDX_Check(pDX, IDC_DIFFBYDOUBLECLICK, m_bDiffByDoubleClick);
	DDX_Check(pDX, IDC_SYSTEMLOCALEFORDATES, m_bUseSystemLocaleForDates);
	DDX_Check(pDX, IDC_USERECYCLEBIN, m_bUseRecycleBin);
}


BEGIN_MESSAGE_MAP(CSetDialogs, ISettingsPropPage)
	ON_BN_CLICKED(IDC_SHORTDATEFORMAT, OnChange)
	ON_BN_CLICKED(IDC_RELATIVETIMES, OnChange)
	ON_BN_CLICKED(IDC_SYSTEMLOCALEFORDATES, OnChange)
	ON_CBN_SELCHANGE(IDC_FONTSIZES, OnChange)
	ON_CBN_SELCHANGE(IDC_FONTNAMES, OnChange)
	ON_CBN_SELCHANGE(IDC_AUTOCLOSECOMBO, OnCbnSelchangeAutoclosecombo)
	ON_BN_CLICKED(IDC_DIFFBYDOUBLECLICK, OnChange)
	ON_BN_CLICKED(IDC_USERECYCLEBIN, OnChange)
END_MESSAGE_MAP()


// CSetDialogs message handlers
BOOL CSetDialogs::OnInitDialog()
{
	CMFCFontComboBox::m_bDrawUsingFont = true;

	ISettingsPropPage::OnInitDialog();

	EnableToolTips();

	int ind = m_cAutoClose.AddString(CString(MAKEINTRESOURCE(IDS_PROGRS_CLOSE_MANUAL)));
	m_cAutoClose.SetItemData(ind, CLOSE_MANUAL);
	ind = m_cAutoClose.AddString(CString(MAKEINTRESOURCE(IDS_PROGRS_CLOSE_NOMERGES)));
	m_cAutoClose.SetItemData(ind, CLOSE_NOMERGES);
	ind = m_cAutoClose.AddString(CString(MAKEINTRESOURCE(IDS_PROGRS_CLOSE_NOCONFLICTS)));
	m_cAutoClose.SetItemData(ind, CLOSE_NOCONFLICTS);
	ind = m_cAutoClose.AddString(CString(MAKEINTRESOURCE(IDS_PROGRS_CLOSE_NOERROR)));
	m_cAutoClose.SetItemData(ind, CLOSE_NOERRORS);
	ind = m_cAutoClose.AddString(CString(MAKEINTRESOURCE(IDS_PROGRS_CLOSE_LOCAL)));
	m_cAutoClose.SetItemData(ind, CLOSE_LOCAL);

	m_dwAutoClose = m_regAutoClose;
	m_bShortDateFormat = m_regShortDateFormat;
	m_bRelativeTimes = m_regRelativeTimes;
	m_bUseSystemLocaleForDates = m_regUseSystemLocaleForDates;
	m_sFontName = m_regFontName;
	m_dwFontSize = m_regFontSize;
	m_bDiffByDoubleClick = m_regDiffByDoubleClick;
	m_bUseRecycleBin = m_regUseRecycleBin;

	for (int i=0; i<m_cAutoClose.GetCount(); ++i)
		if (m_cAutoClose.GetItemData(i)==m_dwAutoClose)
			m_cAutoClose.SetCurSel(i);

	CString temp;

	m_tooltips.Create(this);
	m_tooltips.AddTool(IDC_SHORTDATEFORMAT, IDS_SETTINGS_SHORTDATEFORMAT_TT);
	m_tooltips.AddTool(IDC_RELATIVETIMES, IDS_SETTINGS_RELATIVETIMES_TT);
	m_tooltips.AddTool(IDC_SYSTEMLOCALEFORDATES, IDS_SETTINGS_USESYSTEMLOCALEFORDATES_TT);
	m_tooltips.AddTool(IDC_AUTOCLOSECOMBO, IDS_SETTINGS_AUTOCLOSE_TT);
	m_tooltips.AddTool(IDC_DIFFBYDOUBLECLICK, IDS_SETTINGS_DIFFBYDOUBLECLICK_TT);
	m_tooltips.AddTool(IDC_USERECYCLEBIN, IDS_SETTINGS_USERECYCLEBIN_TT);

	int count = 0;
	for (int i=6; i<32; i=i+2)
	{
		temp.Format(_T("%d"), i);
		m_cFontSizes.AddString(temp);
		m_cFontSizes.SetItemData(count++, i);
	}
	BOOL foundfont = FALSE;
	for (int i=0; i<m_cFontSizes.GetCount(); i++)
	{
		if (m_cFontSizes.GetItemData(i) == m_dwFontSize)
		{
			m_cFontSizes.SetCurSel(i);
			foundfont = TRUE;
		}
	}
	if (!foundfont)
	{
		temp.Format(_T("%d"), m_dwFontSize);
		m_cFontSizes.SetWindowText(temp);
	}

	m_cFontNames.Setup(DEVICE_FONTTYPE|RASTER_FONTTYPE|TRUETYPE_FONTTYPE, 1, FIXED_PITCH);
	m_cFontNames.SelectFont(m_sFontName);

	UpdateData(FALSE);
	return TRUE;
}

BOOL CSetDialogs::PreTranslateMessage(MSG* pMsg)
{
	m_tooltips.RelayEvent(pMsg);
	return ISettingsPropPage::PreTranslateMessage(pMsg);
}

void CSetDialogs::OnChange()
{
	SetModified();
}

BOOL CSetDialogs::OnApply()
{
	UpdateData();
	if (m_cFontNames.GetSelFont())
		m_sFontName = m_cFontNames.GetSelFont()->m_strName;
	else
		m_sFontName = m_regFontName;

    Store (m_dwAutoClose, m_regAutoClose);
	Store (m_bShortDateFormat, m_regShortDateFormat);
	Store (m_bRelativeTimes, m_regRelativeTimes);
    Store (m_bUseSystemLocaleForDates, m_regUseSystemLocaleForDates);

    Store (m_sFontName, m_regFontName);
    Store (m_dwFontSize, m_regFontSize);
	Store (m_bDiffByDoubleClick, m_regDiffByDoubleClick);
	Store (m_bUseRecycleBin, m_regUseRecycleBin);

    SetModified(FALSE);
	return ISettingsPropPage::OnApply();
}

void CSetDialogs::OnCbnSelchangeAutoclosecombo()
{
	if (m_cAutoClose.GetCurSel() != CB_ERR)
	{
		m_dwAutoClose = m_cAutoClose.GetItemData(m_cAutoClose.GetCurSel());
	}
	SetModified();
}
