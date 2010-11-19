/*
 * Copyright © 2004-2008 Jens Oknelid, paskharen@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * In addition, as a special exception, compiling, linking, and/or
 * using OpenSSL with this program is allowed.
 */

#include "finishedtransfers.hh"
#include <client/Text.h>
#include "wulformanager.hh"
#include "WulforUtil.hh"

using namespace std;

FinishedTransfers* FinishedTransfers::createFinishedDownloads()
{
	return new FinishedTransfers(Entry::FINISHED_DOWNLOADS, _("Finished Downloads"), false);
}

FinishedTransfers* FinishedTransfers::createFinishedUploads()
{
	return new FinishedTransfers(Entry::FINISHED_UPLOADS, _("Finished Uploads"), true);
}

FinishedTransfers::FinishedTransfers(const EntryType type, const string &title, bool isUpload):
	BookEntry(type, title, "finishedtransfers.glade"),
	isUpload(isUpload),
	items(0),
	totalBytes(0),
	totalTime(0)
{
	// Initialize transfer treeview
	transferView.setView(GTK_TREE_VIEW(getWidget("view")), true, "finished");
	transferView.insertColumn("Time", G_TYPE_STRING, TreeView::STRING, 150);
	transferView.insertColumn("Filename", G_TYPE_STRING, TreeView::STRING, 100);
	transferView.insertColumn("Path", G_TYPE_STRING, TreeView::STRING, 200);
	transferView.insertColumn("Nick", G_TYPE_STRING, TreeView::STRING, 100);
	transferView.insertColumn("Hub", G_TYPE_STRING, TreeView::STRING, 200);
	transferView.insertColumn("Size", G_TYPE_INT64, TreeView::BYTE, 100);
	transferView.insertColumn("Speed", G_TYPE_INT64, TreeView::SPEED, 100);
	transferView.insertColumn("CRC Checked", G_TYPE_STRING, TreeView::STRING, 100);
	transferView.insertHiddenColumn("Target", G_TYPE_STRING);
	transferView.insertHiddenColumn("Elapsed Time", G_TYPE_INT64);
	transferView.finalize();
	transferStore = gtk_list_store_newv(transferView.getColCount(), transferView.getGTypes());
	gtk_tree_view_set_model(transferView.get(), GTK_TREE_MODEL(transferStore));
	g_object_unref(transferStore);
	transferSelection = gtk_tree_view_get_selection(transferView.get());
	gtk_tree_view_column_set_sort_indicator(gtk_tree_view_get_column(transferView.get(), transferView.col("Time")), TRUE);
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(transferStore), transferView.col("Time"), GTK_SORT_ASCENDING);
	gtk_tree_view_set_fixed_height_mode(transferView.get(), TRUE);
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(transferView.get()), GTK_SELECTION_MULTIPLE);

	// Connect the signals to their callback functions.
	g_signal_connect(getWidget("openItem"), "activate", G_CALLBACK(onOpen_gui), (gpointer)this);
	g_signal_connect(getWidget("openFolderItem"), "activate", G_CALLBACK(onOpenFolder_gui), (gpointer)this);
	g_signal_connect(getWidget("removeItem"), "activate", G_CALLBACK(onRemoveItems_gui), (gpointer)this);
	g_signal_connect(getWidget("removeAllItem"), "activate", G_CALLBACK(onRemoveAll_gui), (gpointer)this);
	g_signal_connect(transferView.get(), "button-press-event", G_CALLBACK(onButtonPressed_gui), (gpointer)this);
	g_signal_connect(transferView.get(), "button-release-event", G_CALLBACK(onButtonReleased_gui), (gpointer)this);
	g_signal_connect(transferView.get(), "key-release-event", G_CALLBACK(onKeyReleased_gui), (gpointer)this);
}

FinishedTransfers::~FinishedTransfers()
{
	FinishedManager::getInstance()->removeListener(this);
}

void FinishedTransfers::show()
{
	initializeList_client();
	//Func0<FinishedTransfers> *func = new Func0<FinishedTransfers>(this, &FinishedTransfers::initializeList_client);
	//WulforManager::get()->dispatchClientFunc(func);
	FinishedManager::getInstance()->addListener(this);
}

void FinishedTransfers::addItem_gui(StringMap params, bool update)
{
	GtkTreeIter iter;
	int64_t size = Util::toInt64(params["Size"]);
	int64_t speed = Util::toInt64(params["Speed"]);
	int64_t time = Util::toInt64(params["Elapsed Time"]);
	totalBytes += size;
	totalTime += time;
	items++;

	gtk_list_store_append(transferStore, &iter);
	gtk_list_store_set(transferStore, &iter,
		transferView.col("Filename"), params["Filename"].c_str(),
		transferView.col("Time"), params["Time"].c_str(),
		transferView.col("Path"), params["Path"].c_str(),
		transferView.col("Nick"), params["Nick"].c_str(),
		transferView.col("Hub"), params["Hub"].c_str(),
		transferView.col("Size"), size, 
		transferView.col("Speed"), speed,
		transferView.col("CRC Checked"), params["CRC Checked"].c_str(),
		transferView.col("Target"), params["Target"].c_str(),
		transferView.col("Elapsed Time"), time,
		-1);

	if (update)
	{
		updateStatus_gui();

		if ((!isUpload && BOOLSETTING(BOLD_FINISHED_DOWNLOADS)) ||
			(isUpload && BOOLSETTING(BOLD_FINISHED_UPLOADS)))
		{
			setBold_gui();
		}
	}
}

void FinishedTransfers::updateStatus_gui()
{
	string status = Util::toString(items) + _(" Items");
	string size = Util::formatBytes(totalBytes);
	string speed = Util::formatBytes((totalTime > 0) ? totalBytes * ((int64_t)1000) / totalTime : 0) + "/s";

	gtk_statusbar_push(GTK_STATUSBAR(getWidget("totalItems")), 0, status.c_str());
	gtk_statusbar_push(GTK_STATUSBAR(getWidget("totalSize")), 0, size.c_str());
	gtk_statusbar_push(GTK_STATUSBAR(getWidget("averageSpeed")), 0, speed.c_str());
}

gboolean FinishedTransfers::onButtonPressed_gui(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	FinishedTransfers *ft = (FinishedTransfers *)data;

	if (event->button == 3)
	{
		GtkTreePath *path;
		if (gtk_tree_view_get_path_at_pos(ft->transferView.get(), (gint)event->x, (gint)event->y, &path, NULL, NULL, NULL))
		{
			bool selected = gtk_tree_selection_path_is_selected(ft->transferSelection, path);
			gtk_tree_path_free(path);

			if (selected)
				return TRUE;
		}
	}

	return FALSE;
}

gboolean FinishedTransfers::onButtonReleased_gui(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	FinishedTransfers *ft = (FinishedTransfers *)data;
	int count = gtk_tree_selection_count_selected_rows(ft->transferSelection);

	if (event->button == 3 && count > 0)
	{
		gtk_menu_popup(GTK_MENU(ft->getWidget("menu")), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
		gtk_widget_show_all(ft->getWidget("menu"));
	}

	return FALSE;
}

gboolean FinishedTransfers::onKeyReleased_gui(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	FinishedTransfers *ft = (FinishedTransfers *)data;
	int count = gtk_tree_selection_count_selected_rows(ft->transferSelection);

	if (count > 0)
	{
		if (event->keyval == GDK_Return || event->keyval == GDK_KP_Enter)
		{
			onOpen_gui(NULL, data);
		}
		else if (event->keyval == GDK_Delete || event->keyval == GDK_BackSpace)
		{
			onRemoveItems_gui(NULL, data);
		}
		else if (event->keyval == GDK_Menu || (event->keyval == GDK_F10 && event->state & GDK_SHIFT_MASK))
		{
			gtk_menu_popup(GTK_MENU(ft->getWidget("menu")), NULL, NULL, NULL, NULL, 1, event->time);
			gtk_widget_show_all(ft->getWidget("menu"));
		}
	}

	return FALSE;
}

void FinishedTransfers::onOpen_gui(GtkMenuItem *item, gpointer data)
{
	FinishedTransfers *ft = (FinishedTransfers *)data;
	int count = gtk_tree_selection_count_selected_rows(ft->transferSelection);

	if (count <= 0)
		return;

	GtkTreeIter iter;
	GtkTreePath *path;
	GList *list = gtk_tree_selection_get_selected_rows(ft->transferSelection, NULL);

	for (GList *i = list; i; i = i->next)
	{
		path = (GtkTreePath *)i->data;
		if (gtk_tree_model_get_iter(GTK_TREE_MODEL(ft->transferStore), &iter, path))
		{
			string target = ft->transferView.getString(&iter, "Target");
			if (!target.empty())
				WulforUtil::openURI(target);
		}
		gtk_tree_path_free(path);
	}
	g_list_free(list);
}

void FinishedTransfers::onOpenFolder_gui(GtkMenuItem *item, gpointer data)
{
	FinishedTransfers *ft = (FinishedTransfers *)data;
	int count = gtk_tree_selection_count_selected_rows(ft->transferSelection);

	if (count <= 0)
		return;

	GtkTreeIter iter;
	GtkTreePath *path;
	GList *list = gtk_tree_selection_get_selected_rows(ft->transferSelection, NULL);

	for (GList *i = list; i; i = i->next)
	{
		path = (GtkTreePath *)i->data;
		if (gtk_tree_model_get_iter(GTK_TREE_MODEL(ft->transferStore), &iter, path))
		{
			string target = ft->transferView.getString(&iter, "Path");
			if (!target.empty())
				WulforUtil::openURI(target);
		}
		gtk_tree_path_free(path);
	}
	g_list_free(list);
}

void FinishedTransfers::onRemoveItems_gui(GtkMenuItem *item, gpointer data)
{
	FinishedTransfers *ft = (FinishedTransfers *)data;
	string target;
	GtkTreeIter iter;
	GtkTreePath *path;
	GList *list = gtk_tree_selection_get_selected_rows(ft->transferSelection, NULL);
	typedef Func1<FinishedTransfers, string> F1;
	F1 *func;

	for (GList *i = list; i; i = i->next)
	{
		path = (GtkTreePath *)i->data;
		if (gtk_tree_model_get_iter(GTK_TREE_MODEL(ft->transferStore), &iter, path))
		{
			target = ft->transferView.getString(&iter, "Target");

			func = new F1(ft, &FinishedTransfers::remove_client, target);
			WulforManager::get()->dispatchClientFunc(func);
		}
		gtk_tree_path_free(path);
	}
	g_list_free(list);

	ft->updateStatus_gui();
}

void FinishedTransfers::removeItem_gui(string target)
{
	GtkTreeIter iter;
	GtkTreeModel *m = GTK_TREE_MODEL(transferStore);
	bool valid = gtk_tree_model_get_iter_first(m, &iter);

	while (valid)
	{
		if (target == transferView.getString(&iter, "Target"))
		{
			totalBytes -= transferView.getValue<gint64>(&iter, "Size");
			totalTime -= transferView.getValue<gint64>(&iter, "Elapsed Time");
			gtk_list_store_remove(transferStore, &iter);
			items--;
			return;
		}
		valid = gtk_tree_model_iter_next(m, &iter);
	}
}

void FinishedTransfers::onRemoveAll_gui(GtkMenuItem *item, gpointer data)
{
	FinishedTransfers *ft = (FinishedTransfers *)data;

	gtk_list_store_clear(ft->transferStore);
	ft->totalBytes = 0;
	ft->totalTime = 0;
	ft->items = 0;
	ft->updateStatus_gui();

	typedef Func0<FinishedTransfers> F0;
	F0 *func = new F0(ft, &FinishedTransfers::removeAll_client);
	WulforManager::get()->dispatchClientFunc(func);
}

void FinishedTransfers::initializeList_client()
{
	StringMap params;
	typedef Func2<FinishedTransfers, StringMap, bool> F2;
	//F2 *func;
	const FinishedItem::List &list = FinishedManager::getInstance()->lockList(isUpload);

	for (FinishedItem::List::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		params.clear();
		getFinishedParams_client(*it, params);
		addItem_gui(params, FALSE);
		//func = new F2(this, &FinishedTransfers::addItem_gui, params, FALSE);
		//WulforManager::get()->dispatchGuiFunc(func);
	}

	FinishedManager::getInstance()->unlockList();

	updateStatus_gui();
	//WulforManager::get()->dispatchGuiFunc(new Func0<FinishedTransfers>(this, &FinishedTransfers::updateStatus_gui));
}

void FinishedTransfers::getFinishedParams_client(FinishedItem *item, StringMap &params)
{
	params["Filename"] = Util::getFileName(item->getTarget());
	params["Time"] = Util::formatTime("%Y-%m-%d %H:%M:%S", item->getTime());
	params["Path"] = Util::getFilePath(item->getTarget());
	params["Nick"] = item->getUser();
	params["Hub"] = item->getHub();
	params["Size"] = Util::toString(item->getSize());
	params["Speed"] = Util::toString(item->getAvgSpeed());
	params["CRC Checked"] = item->getCrc32Checked() ? _("Yes") : _("No");
	params["Target"] = item->getTarget();
	params["Elapsed Time"] = Util::toString(item->getMilliSeconds());
}

void FinishedTransfers::remove_client(std::string target)
{
	FinishedItem *item = FinishedManager::getInstance()->getFinishedItem(target, isUpload);

	if (item)
		FinishedManager::getInstance()->remove(item, isUpload);
}

void FinishedTransfers::removeAll_client()
{
	FinishedManager::getInstance()->removeAll(isUpload);
}

void FinishedTransfers::on(FinishedManagerListener::AddedDl, FinishedItem *item) throw()
{
	if (!isUpload)
	{
		StringMap params;
		getFinishedParams_client(item, params);

		typedef Func2<FinishedTransfers, StringMap, bool> F2;
		F2 *func = new F2(this, &FinishedTransfers::addItem_gui, params, TRUE);
		WulforManager::get()->dispatchGuiFunc(func);
	}
}

void FinishedTransfers::on(FinishedManagerListener::AddedUl, FinishedItem *item) throw()
{
	if (isUpload)
	{
		StringMap params;
		getFinishedParams_client(item, params);

		typedef Func2<FinishedTransfers, StringMap, bool> F2;
		F2 *func = new F2(this, &FinishedTransfers::addItem_gui, params, TRUE);
		WulforManager::get()->dispatchGuiFunc(func);
	}
}

void FinishedTransfers::on(FinishedManagerListener::RemovedDl, FinishedItem *item) throw()
{
	if (!isUpload)
	{
		typedef Func1<FinishedTransfers, string> F1;
		F1 *func = new F1(this, &FinishedTransfers::removeItem_gui, item->getTarget());
		WulforManager::get()->dispatchGuiFunc(func);
	}
}

void FinishedTransfers::on(FinishedManagerListener::RemovedUl, FinishedItem *item) throw()
{
	if (isUpload)
	{
		typedef Func1<FinishedTransfers, string> F1;
		F1 *func = new F1(this, &FinishedTransfers::removeItem_gui, item->getTarget());
		WulforManager::get()->dispatchGuiFunc(func);
	}
}
