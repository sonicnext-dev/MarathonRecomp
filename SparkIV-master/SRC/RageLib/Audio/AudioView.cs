/**********************************************************************\

 RageLib - Audio
 Copyright (C) 2009  Arushan/Aru <oneforaru at gmail.com>
 Copyright (C) 2009  DerPlaya78

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

\**********************************************************************/

using System;
using System.Windows.Forms;

namespace RageLib.Audio
{
    public partial class AudioView : UserControl
    {
        private static bool _AutoPlay;
        private static bool _PlayLooped;
        private int _sortColumn = -1;

        public AudioView()
        {
            InitializeComponent();

            chkAutoPlay.Checked = _AutoPlay;
            chkPlayLooped.Checked = _PlayLooped;

            chkAutoPlay.CheckedChanged += delegate { _AutoPlay = chkAutoPlay.Checked; };
            chkPlayLooped.CheckedChanged += delegate { _PlayLooped = chkPlayLooped.Checked; };
        }

        public event EventHandler PlayClicked
        {
            add { btnPlay.Click += value; }
            remove { btnPlay.Click -= value; }
        }

        public event EventHandler StopClicked
        {
            add { btnStop.Click += value; }
            remove { btnStop.Click -= value; }
        }

        public event EventHandler ExportWAVClicked
        {
            add { tsbExportWave.Click += value; }
            remove { tsbExportWave.Click -= value; }
        }

        public event EventHandler ExportMultichannelWAVClicked
        {
            add { tsbExportMultiChannel.Click += value; }
            remove { tsbExportMultiChannel.Click -= value; }
        }

        public event EventHandler SelectedWaveChanged
        {
            add { listAudioBlocks.SelectedIndexChanged += value; }
            remove { listAudioBlocks.SelectedIndexChanged -= value; }
        }

        public bool AutoPlay
        {
            get { return chkAutoPlay.Checked; }
            set { chkAutoPlay.Checked = value; }
        }

        public bool PlayLooped
        {
            get { return chkPlayLooped.Checked; }
            set { chkPlayLooped.Checked = value; }
        }

        public bool SupportsMultichannelExport
        {
            set
            {
                tsbExportMultiChannel.Enabled = value;
            }
        }

        public void ClearWaves()
        {
            listAudioBlocks.SelectedItems.Clear();
            listAudioBlocks.Items.Clear();
        }

        public void AddWave(AudioWave audioWave)
        {
            ListViewItem lvi = new ListViewItem(audioWave.ToString());
            lvi.Name = audioWave.ToString();
            lvi.Tag = audioWave;

            TimeSpan playTime = audioWave.Length;
            ListViewItem.ListViewSubItem lvisub = new ListViewItem.ListViewSubItem();
            lvisub.Tag = playTime;
            lvisub.Text = playTime.ToString();
            lvi.SubItems.Add(lvisub);

            lvisub = new ListViewItem.ListViewSubItem();
            lvisub.Tag = audioWave.SamplesPerSecond;
            lvisub.Text = audioWave.SamplesPerSecond + " Hz";
            lvi.SubItems.Add(lvisub);

            listAudioBlocks.Items.Add(lvi);
        }

        public AudioWave SelectedWave
        {
            get
            {
                if (listAudioBlocks.SelectedItems.Count == 1)
                {
                    return listAudioBlocks.SelectedItems[0].Tag as AudioWave;
                }
                return null;
            }

            set
            {
                if (value != null)
                {
                    ListViewItem[] items = listAudioBlocks.Items.Find(value.ToString(), false);
                    if (items != null && items.Length == 1)
                    {
                        items[0].Selected = true;
                    }
                }
                else
                {
                    listAudioBlocks.SelectedItems.Clear();
                }
            }
        }

        private void listAudioBlocks_ColumnClick(object sender, ColumnClickEventArgs e)
        {
            if (e.Column != _sortColumn)
            {
                _sortColumn = e.Column;
                listAudioBlocks.Sorting = SortOrder.Ascending;
            }
            else
            {
                listAudioBlocks.Sorting = listAudioBlocks.Sorting == SortOrder.Ascending ? SortOrder.Descending : SortOrder.Ascending;
            }

            listAudioBlocks.ListViewItemSorter = new ListViewItemComparer(e.Column, listAudioBlocks.Sorting == SortOrder.Descending);

            listAudioBlocks.Sort();
        }
    }
}
