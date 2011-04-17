/******************************************************************
 *
 * Project: Explorer++
 * File: SelectColumnsDialog.cpp
 * License: GPL - See COPYING in the top level directory
 *
 * Handles the 'Select Columns' dialog.
 *
 * Written by David Erceg
 * www.explorerplusplus.com
 *
 *****************************************************************/

#include "stdafx.h"
#include "Explorer++_internal.h"
#include "SetDefaultColumnsDialog.h"
#include "MainResource.h"
#include "../ShellBrowser/iShellView.h"
#include "../Helper/Helper.h"
#include "../Helper/ShellHelper.h"
#include "../Helper/RegistrySettings.h"
#include "../Helper/XMLSettings.h"


const TCHAR CSetDefaultColumnsDialogPersistentSettings::SETTINGS_KEY[] = _T("SetDefaultColumns");

CSetDefaultColumnsDialog::CSetDefaultColumnsDialog(HINSTANCE hInstance,
	int iResource,HWND hParent) :
CBaseDialog(hInstance,iResource,hParent,true)
{
	m_psdcdps = &CSetDefaultColumnsDialogPersistentSettings::GetInstance();
}

CSetDefaultColumnsDialog::~CSetDefaultColumnsDialog()
{

}

BOOL CSetDefaultColumnsDialog::OnInitDialog()
{
	HWND hComboBox = GetDlgItem(m_hDlg,IDC_DEFAULTCOLUMNS_COMBOBOX);

	TCHAR szFolderName[MAX_PATH];
	int iPos;

	GetCsidlFolderName(CSIDL_CONTROLS,szFolderName,SHGDN_INFOLDER);
	iPos = static_cast<int>(SendMessage(hComboBox,CB_INSERTSTRING,static_cast<WPARAM>(-1),reinterpret_cast<LPARAM>(szFolderName)));
	m_FolderMap.insert(std::tr1::unordered_map<int,FolderType_t>::value_type(iPos,FOLDER_TYPE_CONTROL_PANEL));

	LoadString(GetInstance(),IDS_DEFAULTCOLUMNS_GENERAL,szFolderName,SIZEOF_ARRAY(szFolderName));
	iPos = static_cast<int>(SendMessage(hComboBox,CB_INSERTSTRING,static_cast<WPARAM>(-1),reinterpret_cast<LPARAM>(szFolderName)));
	m_FolderMap.insert(std::tr1::unordered_map<int,FolderType_t>::value_type(iPos,FOLDER_TYPE_GENERAL));

	GetCsidlFolderName(CSIDL_DRIVES,szFolderName,SHGDN_INFOLDER);
	iPos = static_cast<int>(SendMessage(hComboBox,CB_INSERTSTRING,static_cast<WPARAM>(-1),reinterpret_cast<LPARAM>(szFolderName)));
	m_FolderMap.insert(std::tr1::unordered_map<int,FolderType_t>::value_type(iPos,FOLDER_TYPE_COMPUTER));

	GetCsidlFolderName(CSIDL_CONNECTIONS,szFolderName,SHGDN_INFOLDER);
	iPos = static_cast<int>(SendMessage(hComboBox,CB_INSERTSTRING,static_cast<WPARAM>(-1),reinterpret_cast<LPARAM>(szFolderName)));
	m_FolderMap.insert(std::tr1::unordered_map<int,FolderType_t>::value_type(iPos,FOLDER_TYPE_NETWORK));

	GetCsidlFolderName(CSIDL_NETWORK,szFolderName,SHGDN_INFOLDER);
	iPos = static_cast<int>(SendMessage(hComboBox,CB_INSERTSTRING,static_cast<WPARAM>(-1),reinterpret_cast<LPARAM>(szFolderName)));
	m_FolderMap.insert(std::tr1::unordered_map<int,FolderType_t>::value_type(iPos,FOLDER_TYPE_NETWORK_PLACES));

	GetCsidlFolderName(CSIDL_PRINTERS,szFolderName,SHGDN_INFOLDER);
	iPos = static_cast<int>(SendMessage(hComboBox,CB_INSERTSTRING,static_cast<WPARAM>(-1),reinterpret_cast<LPARAM>(szFolderName)));
	m_FolderMap.insert(std::tr1::unordered_map<int,FolderType_t>::value_type(iPos,FOLDER_TYPE_PRINTERS));

	GetCsidlFolderName(CSIDL_BITBUCKET,szFolderName,SHGDN_INFOLDER);
	iPos = static_cast<int>(SendMessage(hComboBox,CB_INSERTSTRING,static_cast<WPARAM>(-1),reinterpret_cast<LPARAM>(szFolderName)));
	m_FolderMap.insert(std::tr1::unordered_map<int,FolderType_t>::value_type(iPos,FOLDER_TYPE_RECYCLE_BIN));

	SendMessage(hComboBox,CB_SELECTSTRING,static_cast<WPARAM>(-1),reinterpret_cast<LPARAM>(m_psdcdps->m_strFolder.c_str()));

	HWND hListView = GetDlgItem(m_hDlg,IDC_DEFAULTCOLUMNS_LISTVIEW);

	ListView_SetExtendedListViewStyleEx(hListView,
	LVS_EX_CHECKBOXES,LVS_EX_CHECKBOXES);

	LVCOLUMN lvColumn;
	lvColumn.mask	= LVCF_WIDTH;
	lvColumn.cx		= 180;
	ListView_InsertColumn(hListView,0,&lvColumn);

	/* TODO: Show columns for selected folder type. */
	/*int iItem = 0;

	for each(auto Column in m_RealFolderColumnList)
	{
		TCHAR szText[64];
		LoadString(GetInstance(),LookupColumnNameStringIndex(itr->id),szText,SIZEOF_ARRAY(szText));

		LVITEM lvItem;
		lvItem.mask		= LVIF_TEXT|LVIF_PARAM;
		lvItem.iItem	= iItem;
		lvItem.iSubItem	= 0;
		lvItem.pszText	= szText;
		lvItem.lParam	= itr->id;
		ListView_InsertItem(hListView,&lvItem);

		ListView_SetCheckState(hListView,iItem,itr->bChecked);

		iItem++;
	}*/

	ListView_SelectItem(hListView,0,TRUE);
	SetFocus(hListView);

	m_psdcdps->RestoreDialogPosition(m_hDlg,true);

	return 0;
}

void CSetDefaultColumnsDialog::GetResizableControlInformation(CBaseDialog::DialogSizeConstraint &dsc,
	std::list<CResizableDialog::Control_t> &ControlList)
{
	dsc = CBaseDialog::DIALOG_SIZE_CONSTRAINT_NONE;

	CResizableDialog::Control_t Control;

	Control.iID = IDC_COLUMNS_LISTVIEW;
	Control.Type = CResizableDialog::TYPE_RESIZE;
	Control.Constraint = CResizableDialog::CONSTRAINT_NONE;
	ControlList.push_back(Control);

	Control.iID = IDC_COLUMNS_MOVEUP;
	Control.Type = CResizableDialog::TYPE_MOVE;
	Control.Constraint = CResizableDialog::CONSTRAINT_X;
	ControlList.push_back(Control);

	Control.iID = IDC_COLUMNS_MOVEDOWN;
	Control.Type = CResizableDialog::TYPE_MOVE;
	Control.Constraint = CResizableDialog::CONSTRAINT_X;
	ControlList.push_back(Control);

	Control.iID = IDC_STATIC_DESCRIPTION;
	Control.Type = CResizableDialog::TYPE_MOVE;
	Control.Constraint = CResizableDialog::CONSTRAINT_Y;
	ControlList.push_back(Control);

	Control.iID = IDC_DEFAULTCOLUMNS_DESCRIPTION;
	Control.Type = CResizableDialog::TYPE_MOVE;
	Control.Constraint = CResizableDialog::CONSTRAINT_Y;
	ControlList.push_back(Control);

	Control.iID = IDC_DEFAULTCOLUMNS_DESCRIPTION;
	Control.Type = CResizableDialog::TYPE_RESIZE;
	Control.Constraint = CResizableDialog::CONSTRAINT_X;
	ControlList.push_back(Control);

	Control.iID = IDC_STATIC_ETCHEDHORZ;
	Control.Type = CResizableDialog::TYPE_MOVE;
	Control.Constraint = CResizableDialog::CONSTRAINT_Y;
	ControlList.push_back(Control);

	Control.iID = IDC_STATIC_ETCHEDHORZ;
	Control.Type = CResizableDialog::TYPE_RESIZE;
	Control.Constraint = CResizableDialog::CONSTRAINT_X;
	ControlList.push_back(Control);

	Control.iID = IDOK;
	Control.Type = CResizableDialog::TYPE_MOVE;
	Control.Constraint = CResizableDialog::CONSTRAINT_NONE;
	ControlList.push_back(Control);

	Control.iID = IDCANCEL;
	Control.Type = CResizableDialog::TYPE_MOVE;
	Control.Constraint = CResizableDialog::CONSTRAINT_NONE;
	ControlList.push_back(Control);

	Control.iID = IDC_GRIPPER;
	Control.Type = CResizableDialog::TYPE_MOVE;
	Control.Constraint = CResizableDialog::CONSTRAINT_NONE;
	ControlList.push_back(Control);
}

BOOL CSetDefaultColumnsDialog::OnCommand(WPARAM wParam,LPARAM lParam)
{
	switch(HIWORD(wParam))
	{
	case CBN_SELCHANGE:
		OnCbnSelChange();
		break;
	}

	switch(LOWORD(wParam))
	{
	case IDC_DEFAULTCOLUMNS_MOVEUP:
		OnMoveColumn(TRUE);
		break;

	case IDC_DEFAULTCOLUMNS_MOVEDOWN:
		OnMoveColumn(FALSE);
		break;

	case IDOK:
		OnOk();
		break;

	case IDCANCEL:
		OnCancel();
		break;
	}

	return 0;
}

BOOL CSetDefaultColumnsDialog::OnNotify(NMHDR *pnmhdr)
{
	switch(pnmhdr->code)
	{
	case LVN_ITEMCHANGING:
		OnLvnItemChanging(reinterpret_cast<NMLISTVIEW *>(pnmhdr));
		break;
	}

	return 0;
}

BOOL CSetDefaultColumnsDialog::OnClose()
{
	EndDialog(m_hDlg,0);
	return 0;
}

void CSetDefaultColumnsDialog::SaveState()
{
	m_psdcdps->SaveDialogPosition(m_hDlg);

	GetWindowString(GetDlgItem(m_hDlg,IDC_DEFAULTCOLUMNS_COMBOBOX),m_psdcdps->m_strFolder);

	m_psdcdps->m_bStateSaved = TRUE;
}

void CSetDefaultColumnsDialog::OnOk()
{
	/* TODO: Save column state for each of the folder types. */

	EndDialog(m_hDlg,1);
}

void CSetDefaultColumnsDialog::OnCancel()
{
	EndDialog(m_hDlg,0);
}

void CSetDefaultColumnsDialog::OnCbnSelChange()
{
	/* TODO: */
	//GetCurrentDefaultColumnState(m_hDlg);

	HWND hComboBox = GetDlgItem(m_hDlg,IDC_DEFAULTCOLUMNS_COMBOBOX);
	int iSelected = static_cast<int>(SendMessage(hComboBox,CB_GETCURSEL,0,0));

	std::list<Column_t> *pColumnList = NULL;

	auto itr = m_FolderMap.find(iSelected);

	if(itr != m_FolderMap.end())
	{
		switch(itr->second)
		{
		case FOLDER_TYPE_GENERAL:
			pColumnList = &m_RealFolderColumnList;
			break;

		case FOLDER_TYPE_COMPUTER:
			pColumnList = &m_MyComputerColumnList;
			break;

		case FOLDER_TYPE_CONTROL_PANEL:
			pColumnList = &m_ControlPanelColumnList;
			break;

		case FOLDER_TYPE_NETWORK:
			pColumnList = &m_NetworkConnectionsColumnList;
			break;

		case FOLDER_TYPE_NETWORK_PLACES:
			pColumnList = &m_MyNetworkPlacesColumnList;
			break;

		case FOLDER_TYPE_PRINTERS:
			pColumnList = &m_PrintersColumnList;
			break;

		case FOLDER_TYPE_RECYCLE_BIN:
			pColumnList = &m_RecycleBinColumnList;
			break;
		}
	}

	HWND hListView = GetDlgItem(m_hDlg,IDC_DEFAULTCOLUMNS_LISTVIEW);
	ListView_DeleteAllItems(hListView);

	/* TODO: */
	/*int iItem = 0;

	for each(auto Column in *pColumnList)
	{
		TCHAR szText[64];
		LoadString(GetInstance(),LookupColumnNameStringIndex(Column.id),szText,SIZEOF_ARRAY(szText));

		LVITEM lvItem;
		lvItem.mask		= LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem	= iItem;
		lvItem.iSubItem	= 0;
		lvItem.pszText	= szText;
		lvItem.lParam	= Column.id;

		ListView_InsertItem(hListView,&lvItem);

		ListView_SetCheckState(hListView,iItem,Column.bChecked);

		iItem++;
	}*/

	ListView_SelectItem(hListView,0,TRUE);
}

void CSetDefaultColumnsDialog::OnLvnItemChanging(NMLISTVIEW *pnmlv)
{
	if(pnmlv->uNewState & LVIS_SELECTED)
	{
		HWND hListView = GetDlgItem(m_hDlg,IDC_COLUMNS_LISTVIEW);

		LVITEM lvItem;
		lvItem.mask		= LVIF_PARAM;
		lvItem.iItem	= pnmlv->iItem;
		lvItem.iSubItem	= 0;
		ListView_GetItem(hListView,&lvItem);

		/* TODO */
		/*int iDescriptionStringIndex = LookupColumnDescriptionStringIndex((int)lvItem.lParam);

		TCHAR szColumnDescription[128];
		LoadString(GetInstance(),iDescriptionStringIndex,szColumnDescription,
			SIZEOF_ARRAY(szColumnDescription));
		SetDlgItemText(m_hDlg,IDC_COLUMNS_DESCRIPTION,szColumnDescription);*/
	}
}

void CSetDefaultColumnsDialog::OnMoveColumn(bool bUp)
{
	HWND hListView = GetDlgItem(m_hDlg,IDC_COLUMNS_LISTVIEW);

	int iSelected = ListView_GetNextItem(hListView,-1,LVNI_SELECTED);

	if(iSelected != -1)
	{
		if(bUp)
		{
			ListView_SwapItems(hListView,iSelected,iSelected - 1);
		}
		else
		{
			ListView_SwapItems(hListView,iSelected,iSelected + 1);
		}

		SetFocus(hListView);
	}
}

/* TODO: */
//void Explorerplusplus::GetCurrentDefaultColumnState(HWND hDlg)
//{
//	HWND			hComboBox;
//	HWND			hListView;
//	LVITEM			lvItem;
//	list<Column_t>	*pColumnList = NULL;
//	Column_t		Column;
//	int				i = 0;
//
//	hComboBox = GetDlgItem(hDlg,IDC_DEFAULTCOLUMNS_COMBOBOX);
//	hListView = GetDlgItem(hDlg,IDC_DEFAULTCOLUMNS_LISTVIEW);
//
//	if(g_iPreviousTypeSel == g_iControlPanel)
//	{
//		pColumnList = &m_ControlPanelColumnList;
//	}
//	else if(g_iPreviousTypeSel == g_iGeneral)
//	{
//		pColumnList = &m_RealFolderColumnList;
//	}
//	else if(g_iPreviousTypeSel == g_iMyComputer)
//	{
//		pColumnList = &m_MyComputerColumnList;
//	}
//	else if(g_iPreviousTypeSel == g_iNetwork)
//	{
//		pColumnList = &m_NetworkConnectionsColumnList;
//	}
//	else if(g_iPreviousTypeSel == g_iNetworkPlaces)
//	{
//		pColumnList = &m_MyNetworkPlacesColumnList;
//	}
//	else if(g_iPreviousTypeSel == g_iPrinters)
//	{
//		pColumnList = &m_PrintersColumnList;
//	}
//	else if(g_iPreviousTypeSel == g_iRecycleBin)
//	{
//		pColumnList = &m_RecycleBinColumnList;
//	}
//
//	pColumnList->clear();
//
//	for(i = 0;i < ListView_GetItemCount(hListView);i++)
//	{
//		lvItem.mask		= LVIF_PARAM;
//		lvItem.iItem	= i;
//		lvItem.iSubItem	= 0;
//		ListView_GetItem(hListView,&lvItem);
//
//		/* TODO: May need to pull the current column width (instead
//		pf using the default width). */
//		Column.id		= (int)lvItem.lParam;
//		Column.iWidth	= DEFAULT_COLUMN_WIDTH;
//		Column.bChecked	= ListView_GetCheckState(hListView,i);
//		pColumnList->push_back(Column);
//	}
//}

CSetDefaultColumnsDialogPersistentSettings::CSetDefaultColumnsDialogPersistentSettings() :
CDialogSettings(SETTINGS_KEY)
{
	m_strFolder = _T("General");
}

CSetDefaultColumnsDialogPersistentSettings::~CSetDefaultColumnsDialogPersistentSettings()
{
	
}

CSetDefaultColumnsDialogPersistentSettings& CSetDefaultColumnsDialogPersistentSettings::GetInstance()
{
	static CSetDefaultColumnsDialogPersistentSettings mfdps;
	return mfdps;
}

void CSetDefaultColumnsDialogPersistentSettings::SaveExtraRegistrySettings(HKEY hKey)
{
	NRegistrySettings::SaveStringToRegistry(hKey,_T("Folder"),m_strFolder.c_str());
}

void CSetDefaultColumnsDialogPersistentSettings::LoadExtraRegistrySettings(HKEY hKey)
{
	NRegistrySettings::ReadStringFromRegistry(hKey,_T("Folder"),m_strFolder);
}

void CSetDefaultColumnsDialogPersistentSettings::SaveExtraXMLSettings(MSXML2::IXMLDOMDocument *pXMLDom,
	MSXML2::IXMLDOMElement *pParentNode)
{
	NXMLSettings::AddAttributeToNode(pXMLDom,pParentNode,_T("Folder"),m_strFolder.c_str());
}

void CSetDefaultColumnsDialogPersistentSettings::LoadExtraXMLSettings(BSTR bstrName,BSTR bstrValue)
{
	if(lstrcmpi(bstrName,_T("Folder")) == 0)
	{
		m_strFolder = _bstr_t(bstrValue);
	}
}