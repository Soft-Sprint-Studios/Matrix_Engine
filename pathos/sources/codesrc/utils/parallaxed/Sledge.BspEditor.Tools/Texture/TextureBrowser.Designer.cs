namespace Sledge.BspEditor.Tools.Texture
{
    partial class TextureBrowser
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            PackageTree = new System.Windows.Forms.TreeView();
            panel1 = new System.Windows.Forms.Panel();
            Triggershow = new System.Windows.Forms.CheckBox();
            SortDescendingCheckbox = new System.Windows.Forms.CheckBox();
            SortOrderCombo = new System.Windows.Forms.ComboBox();
            SortByLabel = new System.Windows.Forms.Label();
            SizeCombo = new System.Windows.Forms.ComboBox();
            TextureSizeLabel = new System.Windows.Forms.Label();
            TextureNameLabel = new System.Windows.Forms.Label();
            UsedTexturesOnlyBox = new System.Windows.Forms.CheckBox();
            SizeLabel = new System.Windows.Forms.Label();
            FilterTextbox = new System.Windows.Forms.TextBox();
            FilterLabel = new System.Windows.Forms.Label();
            FavouritesTree = new System.Windows.Forms.TreeView();
            LeftbarPanel = new System.Windows.Forms.Panel();
            DeleteFavouriteFolderButton = new System.Windows.Forms.Button();
            RemoveFavouriteItemButton = new System.Windows.Forms.Button();
            AddFavouriteFolderButton = new System.Windows.Forms.Button();
            FavouriteTexturesLabel = new System.Windows.Forms.Label();
            TextureListPanel = new System.Windows.Forms.Panel();
            panel1.SuspendLayout();
            LeftbarPanel.SuspendLayout();
            SuspendLayout();
            // 
            // PackageTree
            // 
            PackageTree.Dock = System.Windows.Forms.DockStyle.Top;
            PackageTree.HideSelection = false;
            PackageTree.Location = new System.Drawing.Point(0, 0);
            PackageTree.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            PackageTree.Name = "PackageTree";
            PackageTree.Size = new System.Drawing.Size(264, 476);
            PackageTree.TabIndex = 1;
            PackageTree.AfterSelect += SelectedPackageChanged;
            // 
            // panel1
            // 
            panel1.Controls.Add(Triggershow);
            panel1.Controls.Add(SortDescendingCheckbox);
            panel1.Controls.Add(SortOrderCombo);
            panel1.Controls.Add(SortByLabel);
            panel1.Controls.Add(SizeCombo);
            panel1.Controls.Add(TextureSizeLabel);
            panel1.Controls.Add(TextureNameLabel);
            panel1.Controls.Add(UsedTexturesOnlyBox);
            panel1.Controls.Add(SizeLabel);
            panel1.Controls.Add(FilterTextbox);
            panel1.Controls.Add(FilterLabel);
            panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            panel1.Location = new System.Drawing.Point(0, 571);
            panel1.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            panel1.Name = "panel1";
            panel1.Size = new System.Drawing.Size(1097, 81);
            panel1.TabIndex = 2;
            // 
            // Triggershow
            // 
            Triggershow.AutoSize = true;
            Triggershow.Checked = true;
            Triggershow.CheckState = System.Windows.Forms.CheckState.Checked;
            Triggershow.Location = new System.Drawing.Point(315, 38);
            Triggershow.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            Triggershow.Name = "Triggershow";
            Triggershow.Size = new System.Drawing.Size(98, 19);
            Triggershow.TabIndex = 11;
            Triggershow.Text = "Show triggers";
            Triggershow.UseVisualStyleBackColor = true;
            Triggershow.CheckedChanged += TriggerShowChanged;
            // 
            // SortDescendingCheckbox
            // 
            SortDescendingCheckbox.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right;
            SortDescendingCheckbox.AutoSize = true;
            SortDescendingCheckbox.Location = new System.Drawing.Point(967, 40);
            SortDescendingCheckbox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            SortDescendingCheckbox.Name = "SortDescendingCheckbox";
            SortDescendingCheckbox.Size = new System.Drawing.Size(111, 19);
            SortDescendingCheckbox.TabIndex = 10;
            SortDescendingCheckbox.Text = "Sort descending";
            SortDescendingCheckbox.UseVisualStyleBackColor = true;
            SortDescendingCheckbox.CheckedChanged += SortDescendingCheckboxChanged;
            // 
            // SortOrderCombo
            // 
            SortOrderCombo.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right;
            SortOrderCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            SortOrderCombo.FormattingEnabled = true;
            SortOrderCombo.Location = new System.Drawing.Point(958, 10);
            SortOrderCombo.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            SortOrderCombo.Name = "SortOrderCombo";
            SortOrderCombo.Size = new System.Drawing.Size(124, 23);
            SortOrderCombo.TabIndex = 9;
            SortOrderCombo.SelectedIndexChanged += SortOrderComboIndexChanged;
            // 
            // SortByLabel
            // 
            SortByLabel.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right;
            SortByLabel.Location = new System.Drawing.Point(793, 10);
            SortByLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            SortByLabel.Name = "SortByLabel";
            SortByLabel.Size = new System.Drawing.Size(158, 24);
            SortByLabel.TabIndex = 8;
            SortByLabel.Text = "Sort By";
            SortByLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // SizeCombo
            // 
            SizeCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            SizeCombo.FormattingEnabled = true;
            SizeCombo.Items.AddRange(new object[] { "64", "128", "256", "512" });
            SizeCombo.Location = new System.Drawing.Point(99, 39);
            SizeCombo.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            SizeCombo.Name = "SizeCombo";
            SizeCombo.Size = new System.Drawing.Size(208, 23);
            SizeCombo.TabIndex = 7;
            SizeCombo.SelectedIndexChanged += SizeValueChanged;
            // 
            // TextureSizeLabel
            // 
            TextureSizeLabel.AutoSize = true;
            TextureSizeLabel.Location = new System.Drawing.Point(498, 44);
            TextureSizeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            TextureSizeLabel.Name = "TextureSizeLabel";
            TextureSizeLabel.Size = new System.Drawing.Size(27, 15);
            TextureSizeLabel.TabIndex = 6;
            TextureSizeLabel.Text = "Size";
            // 
            // TextureNameLabel
            // 
            TextureNameLabel.AutoSize = true;
            TextureNameLabel.Location = new System.Drawing.Point(498, 13);
            TextureNameLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            TextureNameLabel.Name = "TextureNameLabel";
            TextureNameLabel.Size = new System.Drawing.Size(39, 15);
            TextureNameLabel.TabIndex = 6;
            TextureNameLabel.Text = "Name";
            // 
            // UsedTexturesOnlyBox
            // 
            UsedTexturesOnlyBox.AutoSize = true;
            UsedTexturesOnlyBox.Location = new System.Drawing.Point(315, 13);
            UsedTexturesOnlyBox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            UsedTexturesOnlyBox.Name = "UsedTexturesOnlyBox";
            UsedTexturesOnlyBox.Size = new System.Drawing.Size(123, 19);
            UsedTexturesOnlyBox.TabIndex = 4;
            UsedTexturesOnlyBox.Text = "Used textures only";
            UsedTexturesOnlyBox.UseVisualStyleBackColor = true;
            UsedTexturesOnlyBox.CheckedChanged += UsedTexturesOnlyChanged;
            // 
            // SizeLabel
            // 
            SizeLabel.Location = new System.Drawing.Point(4, 39);
            SizeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            SizeLabel.Name = "SizeLabel";
            SizeLabel.Size = new System.Drawing.Size(89, 24);
            SizeLabel.TabIndex = 2;
            SizeLabel.Text = "Size";
            SizeLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // FilterTextbox
            // 
            FilterTextbox.HideSelection = false;
            FilterTextbox.Location = new System.Drawing.Point(99, 9);
            FilterTextbox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            FilterTextbox.Name = "FilterTextbox";
            FilterTextbox.Size = new System.Drawing.Size(208, 23);
            FilterTextbox.TabIndex = 1;
            FilterTextbox.KeyUp += FilterTextboxKeyUp;
            // 
            // FilterLabel
            // 
            FilterLabel.Location = new System.Drawing.Point(4, 9);
            FilterLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            FilterLabel.Name = "FilterLabel";
            FilterLabel.Size = new System.Drawing.Size(89, 23);
            FilterLabel.TabIndex = 0;
            FilterLabel.Text = "Filter";
            FilterLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // FavouritesTree
            // 
            FavouritesTree.AllowDrop = true;
            FavouritesTree.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
            FavouritesTree.HideSelection = false;
            FavouritesTree.Location = new System.Drawing.Point(0, 498);
            FavouritesTree.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            FavouritesTree.Name = "FavouritesTree";
            FavouritesTree.Size = new System.Drawing.Size(263, 14);
            FavouritesTree.TabIndex = 1;
            FavouritesTree.AfterSelect += SelectedFavouriteChanged;
            FavouritesTree.DragDrop += FavouritesTreeDragDrop;
            FavouritesTree.DragEnter += FavouritesTreeDragEnter;
            FavouritesTree.DragOver += FavouritesTreeDragOver;
            FavouritesTree.DragLeave += FavouritesTreeDragLeave;
            // 
            // LeftbarPanel
            // 
            LeftbarPanel.Controls.Add(DeleteFavouriteFolderButton);
            LeftbarPanel.Controls.Add(RemoveFavouriteItemButton);
            LeftbarPanel.Controls.Add(AddFavouriteFolderButton);
            LeftbarPanel.Controls.Add(FavouriteTexturesLabel);
            LeftbarPanel.Controls.Add(FavouritesTree);
            LeftbarPanel.Controls.Add(PackageTree);
            LeftbarPanel.Dock = System.Windows.Forms.DockStyle.Left;
            LeftbarPanel.Location = new System.Drawing.Point(0, 0);
            LeftbarPanel.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            LeftbarPanel.Name = "LeftbarPanel";
            LeftbarPanel.Size = new System.Drawing.Size(264, 571);
            LeftbarPanel.TabIndex = 3;
            // 
            // DeleteFavouriteFolderButton
            // 
            DeleteFavouriteFolderButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left;
            DeleteFavouriteFolderButton.Enabled = false;
            DeleteFavouriteFolderButton.Location = new System.Drawing.Point(132, 515);
            DeleteFavouriteFolderButton.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            DeleteFavouriteFolderButton.Name = "DeleteFavouriteFolderButton";
            DeleteFavouriteFolderButton.Size = new System.Drawing.Size(128, 27);
            DeleteFavouriteFolderButton.TabIndex = 3;
            DeleteFavouriteFolderButton.Text = "Delete Folder";
            DeleteFavouriteFolderButton.UseVisualStyleBackColor = true;
            DeleteFavouriteFolderButton.Click += DeleteFavouriteFolderButtonClicked;
            // 
            // RemoveFavouriteItemButton
            // 
            RemoveFavouriteItemButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left;
            RemoveFavouriteItemButton.Enabled = false;
            RemoveFavouriteItemButton.Location = new System.Drawing.Point(4, 541);
            RemoveFavouriteItemButton.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            RemoveFavouriteItemButton.Name = "RemoveFavouriteItemButton";
            RemoveFavouriteItemButton.Size = new System.Drawing.Size(257, 27);
            RemoveFavouriteItemButton.TabIndex = 3;
            RemoveFavouriteItemButton.Text = "Remove Selection From Folder";
            RemoveFavouriteItemButton.UseVisualStyleBackColor = true;
            RemoveFavouriteItemButton.Click += RemoveFavouriteItemButtonClicked;
            // 
            // AddFavouriteFolderButton
            // 
            AddFavouriteFolderButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left;
            AddFavouriteFolderButton.Enabled = false;
            AddFavouriteFolderButton.Location = new System.Drawing.Point(4, 515);
            AddFavouriteFolderButton.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            AddFavouriteFolderButton.Name = "AddFavouriteFolderButton";
            AddFavouriteFolderButton.Size = new System.Drawing.Size(128, 27);
            AddFavouriteFolderButton.TabIndex = 3;
            AddFavouriteFolderButton.Text = "Add Folder";
            AddFavouriteFolderButton.UseVisualStyleBackColor = true;
            AddFavouriteFolderButton.Click += AddFavouriteFolderButtonClicked;
            // 
            // FavouriteTexturesLabel
            // 
            FavouriteTexturesLabel.AutoSize = true;
            FavouriteTexturesLabel.Location = new System.Drawing.Point(4, 480);
            FavouriteTexturesLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            FavouriteTexturesLabel.Name = "FavouriteTexturesLabel";
            FavouriteTexturesLabel.Size = new System.Drawing.Size(188, 15);
            FavouriteTexturesLabel.TabIndex = 2;
            FavouriteTexturesLabel.Text = "Favourite Textures (drag and drop)";
            // 
            // TextureListPanel
            // 
            TextureListPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            TextureListPanel.Location = new System.Drawing.Point(264, 0);
            TextureListPanel.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            TextureListPanel.Name = "TextureListPanel";
            TextureListPanel.Size = new System.Drawing.Size(833, 571);
            TextureListPanel.TabIndex = 4;
            // 
            // TextureBrowser
            // 
            AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            ClientSize = new System.Drawing.Size(1097, 652);
            Controls.Add(TextureListPanel);
            Controls.Add(LeftbarPanel);
            Controls.Add(panel1);
            KeyPreview = true;
            Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            MinimizeBox = false;
            Name = "TextureBrowser";
            Text = "Texture Browser";
            WindowState = System.Windows.Forms.FormWindowState.Maximized;
            KeyDown += TextureBrowserKeyDown;
            KeyPress += TextureBrowserKeyPress;
            panel1.ResumeLayout(false);
            panel1.PerformLayout();
            LeftbarPanel.ResumeLayout(false);
            LeftbarPanel.PerformLayout();
            ResumeLayout(false);
        }

        #endregion

        private System.Windows.Forms.TreeView PackageTree;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label TextureSizeLabel;
        private System.Windows.Forms.Label TextureNameLabel;
        private System.Windows.Forms.CheckBox UsedTexturesOnlyBox;
        private System.Windows.Forms.CheckBox Triggershow;
        private System.Windows.Forms.Label SizeLabel;
        private System.Windows.Forms.TextBox FilterTextbox;
        private System.Windows.Forms.Label FilterLabel;
        private System.Windows.Forms.ComboBox SizeCombo;
        private System.Windows.Forms.ComboBox SortOrderCombo;
        private System.Windows.Forms.Label SortByLabel;
        private System.Windows.Forms.CheckBox SortDescendingCheckbox;
        private System.Windows.Forms.TreeView FavouritesTree;
        private System.Windows.Forms.Panel LeftbarPanel;
        private System.Windows.Forms.Button DeleteFavouriteFolderButton;
        private System.Windows.Forms.Button AddFavouriteFolderButton;
        private System.Windows.Forms.Label FavouriteTexturesLabel;
        private System.Windows.Forms.Button RemoveFavouriteItemButton;
        private System.Windows.Forms.Panel TextureListPanel;
    }
}