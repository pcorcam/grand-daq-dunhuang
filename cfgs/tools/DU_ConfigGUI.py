import tkinter as tk
from tkinter import ttk
import yaml

class ConfigEditor(tk.Tk): ## inherit the tk class
    def __init__(self, filename):
        super().__init__()
        self.title("DU Configuration Editor")
        self.config_data = self.load_config(filename)
        self.create_widgets() ## create all widgets within the GUI
        
    def load_config(self, filename):
        with open(filename, 'r') as file:
            config = yaml.safe_load(file)
        return config

    def save_config(self):
        with open("/home/grand/workarea/grand-daq/cfgs/DU-readable-conf_testGUI.yaml", 'w') as file:
            yaml.dump(self.config_data, file)
    
    def create_widgets(self):
        # Destroy exisiting widgets before recreating the wgole GUI
        for widget in self.winfo_children():
            widget.destroy()
            
        # Create a frame for the scrollable area
        scroll_frame = ttk.Frame(self)
        scroll_frame.pack(fill='both', expand=True)
        
        # Create canvas and scrollbar
        canvas = tk.Canvas(scroll_frame)
        scrollbar = ttk.Scrollbar(scroll_frame, orient="vertical", command=canvas.yview)
        scrollbar.pack(side='right', fill='y')
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
            print("section is ", section)
            ttk.Label(inner_frame, text=section, font=('Helvetica', 12, 'bold')).grid(row=row, column=0, columnspan=2, pady=5)
            row += 1
            if section == "CommonConfig":
                for key, value in items.items():
                    if key == "ifopen" and value == 1:
                        COMMON = True   
                    else:
                        COMMON = False
                    if COMMON == True:
                        ttk.Label(inner_frame, text=key).grid(row=row, column=0, padx=5)
                        entry = ttk.Entry(inner_frame, width=30)
                        entry.insert(0, value)
                        entry.grid(row=row, column=1, padx=5)
                        setattr(self, f"{section}_{key}", entry)
                        row += 1
            
            if section == "SpecialConfig":
                
            # if isinstance(items, list):
            #     print("items is ", items)
            #     ttk.Label(inner_frame, text=section, font=('Helvetica', 12, 'bold')).grid(row=row, column=0, columnspan=2, pady=5)
            #     row += 1
            #     for item in items:
            #         ttk.Label(inner_frame, text=section, font=('Helvetica', 12, 'bold')).grid(row=row, column=0, columnspan=2, pady=5)
            #         row += 1
            #         for key,value in item.items():
            #             if key == "ifopen":
            #                 print("key value is ", value)
            #                 break
            #             ttk.Label(inner_frame, text=key).grid(row=row, column=0, columnspan=2, pady=5)
            #             entry =ttk.Entry(inner_frame, width=30)
            #             entry.insert(0, value)
            #             entry.grid(row=row, column=1, padx=5)
            #             setattr(self, f"{section}_{key}", entry)
            #             row += 1
                        
# Create and run GUI interfaceclear
app = ConfigEditor("/home/grand/workarea/grand-daq/cfgs/DU-readable-conf_testGUI.yaml")
app.mainloop()
            