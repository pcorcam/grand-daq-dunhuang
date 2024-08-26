import tkinter as tk
from tkinter import ttk
import yaml

class ConfigEditor(tk.Tk):
    def __init__(self, filename):
        super().__init__()
        self.title("System Configuration Editor")
        self.config_data = self.load_config(filename)
        self.create_widgets()

    def load_config(self, filename):
        with open(filename, 'r') as file:
            config = yaml.safe_load(file)
        return config

    def save_config(self):
        with open("/home/grand/workarea/grand-daq/cfgs/sysconfig_2.yaml", 'w') as file:
            yaml.dump(self.config_data, file)

    def create_widgets(self):
        # Destroy existing widgets before recreating
        for widget in self.winfo_children():
            widget.destroy()

        # Create a frame for the scrollable area
        scroll_frame = ttk.Frame(self)
        scroll_frame.pack(fill='both', expand=True)

        # Create canvas and scrollbar
        canvas = tk.Canvas(scroll_frame)
        scrollbar = ttk.Scrollbar(scroll_frame, orient="vertical", command=canvas.yview)
        scrollbar.pack(side="right", fill="y")
        canvas.pack(side="left", fill="both", expand=True)
        canvas.configure(yscrollcommand=scrollbar.set)

        # Create another frame inside canvas
        inner_frame = ttk.Frame(canvas)
        canvas.create_window((0, 0), window=inner_frame, anchor="nw")

        # Function to update canvas scroll region
        def on_configure(event):
            canvas.configure(scrollregion=canvas.bbox("all"))

        inner_frame.bind("<Configure>", on_configure)

        # Add labels and entry widgets for dataUnits
        row = 0
        for section, items in self.config_data.items():
            if isinstance(items, list):
                ttk.Label(inner_frame, text=section, font=('Helvetica', 12, 'bold')).grid(row=row, column=0, columnspan=2, pady=5)
                row += 1
                print("items is ", items)
                for item in items:
                    ttk.Label(inner_frame, text=section, font=('Helvetica', 12, 'bold')).grid(row=row, column=0, columnspan=2, pady=5)
                    row += 1
                    for key, value in item.items():
                        ttk.Label(inner_frame, text=key).grid(row=row, column=0, padx=5)
                        entry = ttk.Entry(inner_frame, width=30)
                        entry.insert(0, value)
                        entry.grid(row=row, column=1, padx=5)
                        setattr(self, f"{section}_{key}", entry)
                        row += 1
            else:
                ttk.Label(inner_frame, text=section, font=('Helvetica', 12, 'bold')).grid(row=row, column=0, columnspan=2, pady=5)
                row += 1
                for key, value in items.items():
                    ttk.Label(inner_frame, text=key).grid(row=row, column=0, padx=5)
                    entry = ttk.Entry(inner_frame, width=30)
                    entry.insert(0, value)
                    entry.grid(row=row, column=1, padx=5)
                    setattr(self, f"{section}_{key}", entry)
                    row += 1
                    
        # Add Entry for editing global key
        new_key_label = ttk.Label(self, text="Global New Key:")
        new_key_label.pack(pady=5)
        self.new_key_entry = ttk.Entry(self, width=30)
        self.new_key_entry.pack()

        # Add buttons for functionality
        ttk.Button(self, text="Add DataUnits", command=self.add_data_units).pack(side="left", padx=5, pady=10)
        ttk.Button(self, text="Add Global", command=self.add_global).pack(side="left", padx=5, pady=10)
        ttk.Button(self, text="Delete DataUnits", command=self.delete_data_units).pack(side="left", padx=5, pady=10)
        ttk.Button(self, text="Delete Global", command=self.delete_global).pack(side="left", padx=5, pady=10)
        ttk.Button(self, text="Save", command=self.save_config).pack(side="left", padx=5, pady=10)

    def add_data_units(self):
        self.config_data['dataUnits'].append({"ID": "", "ip": "", "port": "", "type": ""})
        self.create_widgets()

    def add_global(self):
        new_key = self.new_key_entry.get()
        if new_key:
            self.config_data['global'][new_key] = "new_value"
            self.create_widgets()

    def delete_data_units(self):
        if self.config_data['dataUnits']:
            self.config_data['dataUnits'].pop()
            self.create_widgets()

    def delete_global(self):
        if self.config_data['global']:
            last_key = list(self.config_data['global'].keys())[-1]
            del self.config_data['global'][last_key]
            self.create_widgets()
# Create and run GUI interface
app = ConfigEditor("/home/grand/workarea/grand-daq/cfgs/sysconfig_2.yaml")
app.mainloop()
