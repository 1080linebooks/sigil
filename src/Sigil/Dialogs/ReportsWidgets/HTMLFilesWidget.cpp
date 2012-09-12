/************************************************************************
**
**  Copyright (C) 2012 John Schember <john@nachtimwald.com>
**  Copyright (C) 2012 Dave Heiland
**
**  This file is part of Sigil.
**
**  Sigil is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Sigil is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include <QtCore/QFile>
#include <QtGui/QFont>

#include "Misc/NumericItem.h"
#include "ResourceObjects/HTMLResource.h"
#include "Misc/HTMLSpellCheck.h"
#include "Misc/SettingsStore.h"
#include "HTMLFilesWidget.h"

HTMLFilesWidget::HTMLFilesWidget(QList<Resource*> html_resources, QSharedPointer< Book > book)
    :
    m_HTMLResources(html_resources),
    m_Book(book),
    m_ItemModel(new QStandardItemModel)
{

    ui.setupUi(this);
    connectSignalsSlots();

    SetupTable();
}

void HTMLFilesWidget::SetupTable(int sort_column, Qt::SortOrder sort_order)
{
    m_ItemModel->clear();

    QStringList header;

    header.append(tr("Name"));
    header.append(tr("File Size (KB)"));
    header.append(tr("All Words"));
    header.append(tr("Misspelled Words"));
    header.append(tr("Images"));
    header.append(tr("Stylesheets"));

    m_ItemModel->setHorizontalHeaderLabels(header);

    ui.fileTree->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui.fileTree->setModel(m_ItemModel);

    ui.fileTree->header()->setSortIndicatorShown(true);

    double total_size = 0;
    int total_all_words = 0;
    int total_misspelled_words = 0;
    int total_images = 0;
    int total_stylesheets = 0;

    QHash<QString, QStringList> stylesheet_names_hash = m_Book->GetStylesheetsInHTMLFiles();
    QHash<QString, QStringList> image_names_hash = m_Book->GetImagesInHTMLFiles();

    foreach (Resource *resource, m_HTMLResources) {
            HTMLResource *html_resource = qobject_cast<HTMLResource *>(resource);

            QString filepath = "../" + resource->GetRelativePathToOEBPS();
            QString path = resource->GetFullPath();

            QList<QStandardItem *> rowItems;

            // Filename
            QStandardItem *name_item = new QStandardItem();
            name_item->setText(resource->Filename());
            name_item->setToolTip(filepath);
            rowItems << name_item;

            // File Size
            double ffsize = QFile(path).size() / 1024.0;
            total_size += ffsize;
            QString fsize = QLocale().toString(ffsize, 'f', 2);
            NumericItem *size_item = new NumericItem();
            size_item->setText(fsize);
            rowItems << size_item;

            // All words
            int all_words = HTMLSpellCheck::CountAllWords(html_resource->GetText());
            total_all_words += all_words;
            NumericItem *words_item = new NumericItem();
            words_item->setText(QString::number(all_words));
            rowItems << words_item;

            // Misspelled words
            int misspelled_words = HTMLSpellCheck::CountMisspelledWords(html_resource->GetText());
            total_misspelled_words += misspelled_words;
            NumericItem *misspelled_item = new NumericItem();
            misspelled_item->setText(QString::number(misspelled_words));
            rowItems << misspelled_item;

            // Images
            NumericItem *image_item = new NumericItem();
            QStringList image_names = image_names_hash[resource->Filename()];
            total_images += image_names.count();
            image_item->setText(QString::number(image_names.count()));
            if (!image_names.isEmpty()) {
                image_item->setToolTip(image_names.join("\n"));
            }
            rowItems << image_item;

            // Linked Stylesheets
            NumericItem *stylesheet_item = new NumericItem();
            QStringList stylesheet_names = stylesheet_names_hash[resource->Filename()];
            total_stylesheets += stylesheet_names.count();
            stylesheet_item->setText(QString::number(stylesheet_names.count()));
            if (!stylesheet_names.isEmpty()) {
                stylesheet_item->setToolTip(stylesheet_names.join("\n"));
            }
            rowItems << stylesheet_item;

            for (int i = 0; i < rowItems.count(); i++) {
                rowItems[i]->setEditable(false);
            }
            m_ItemModel->appendRow(rowItems);
    }

    // Sort before adding the totals row
    // Since sortIndicator calls this routine, must disconnect/reconnect while resorting
    disconnect (ui.fileTree->header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(Sort(int, Qt::SortOrder)));
    ui.fileTree->header()->setSortIndicator(sort_column, sort_order);
    connect (ui.fileTree->header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(Sort(int, Qt::SortOrder)));

    // Totals
    NumericItem *nitem;
    QList<QStandardItem *> rowItems;

    // Files
    nitem = new NumericItem();
    nitem->setText(QString::number(m_HTMLResources.count()) % tr(" files"));
    rowItems << nitem;

    // File size
    nitem = new NumericItem();
    nitem->setText(QLocale().toString(total_size, 'f', 2));
    rowItems << nitem;

    // All Words
    nitem = new NumericItem();
    nitem->setText(QString::number(total_all_words));
    rowItems << nitem;

    // Misspelled Words
    nitem = new NumericItem();
    nitem->setText(QString::number(total_misspelled_words));
    rowItems << nitem;

    // Images
    nitem = new NumericItem();
    nitem->setText(QString::number(total_images));
    rowItems << nitem;

    // Stylesheets
    nitem = new NumericItem();
    nitem->setText(QString::number(total_stylesheets));
    rowItems << nitem;

    QFont font = *new QFont();
    font.setWeight(QFont::Bold);
    for (int i = 0; i < rowItems.count(); i++) {
        rowItems[i]->setEditable(false);
        rowItems[i]->setFont(font);
    }

    m_ItemModel->appendRow(rowItems);

    for (int i = 0; i < ui.fileTree->header()->count(); i++) {
        ui.fileTree->resizeColumnToContents(i);
    }
}


void HTMLFilesWidget::FilterEditTextChangedSlot(const QString &text)
{
    const QString lowercaseText = text.toLower();

    QStandardItem *root_item = m_ItemModel->invisibleRootItem();
    QModelIndex parent_index;

    // Hide rows that don't contain the filter text
    int first_visible_row = -1;
    for (int row = 0; row < root_item->rowCount(); row++) {
        if (text.isEmpty() || root_item->child(row, 0)->text().toLower().contains(lowercaseText)) {
            ui.fileTree->setRowHidden(row, parent_index, false);
            if (first_visible_row == -1) {
                first_visible_row = row;
            }
        }
        else {
            ui.fileTree->setRowHidden(row, parent_index, true);
        }
    }

    if (!text.isEmpty() && first_visible_row != -1) {
        // Select the first non-hidden row
        ui.fileTree->setCurrentIndex(root_item->child(first_visible_row, 0)->index());
    }
    else {
        // Clear current and selection, which clears preview image
        ui.fileTree->setCurrentIndex(QModelIndex());
    }
}

void HTMLFilesWidget::Sort(int logicalindex, Qt::SortOrder order)
{
    SetupTable(logicalindex, order);
}

void HTMLFilesWidget::Done()
{
    close();
}

QString HTMLFilesWidget::saveSettings()
{
    QString selected_file;

    if (ui.fileTree->selectionModel()->hasSelection()) {
        QModelIndex index = ui.fileTree->selectionModel()->selectedRows(0).first();
        if (index.row() != m_ItemModel->rowCount() - 1) {
            selected_file = m_ItemModel->itemFromIndex(index)->text();
        }
    }
    return selected_file;
}

void HTMLFilesWidget::connectSignalsSlots()
{
    connect(ui.leFilter,  SIGNAL(textChanged(QString)), 
            this,         SLOT(FilterEditTextChangedSlot(QString)));
//    connect (ui.fileTree, SIGNAL(doubleClicked(const QModelIndex &)),
//            this,         SLOT(accept()));
    connect (ui.fileTree->header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(Sort(int, Qt::SortOrder)));
}