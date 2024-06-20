import tkinter as tk
from tkinter import messagebox
import subprocess
import os

class ExperimentApp:
    def __init__(self, root):
        self.root = root
        self.root.title("My Experiment Application")

        # Title Label
        self.title_label = tk.Label(root, text="My Experiment Application", font=("Arial", 16))
        self.title_label.pack(pady=10)

        # Number of Processes
        self.num_processes_label = tk.Label(root, text="Number of Processes:")
        self.num_processes_label.pack()
        self.num_processes_entry = tk.Entry(root)
        self.num_processes_entry.pack()
        self.num_processes_entry.bind("<KeyRelease>", self.update_process_fields)

        # Time Quantum for Round Robin
        self.time_quantum_label = tk.Label(root, text="Time Quantum (ms):")
        self.time_quantum_label.pack()
        self.time_quantum_entry = tk.Entry(root)
        self.time_quantum_entry.pack()

        # Process Fields Frame
        self.process_fields_frame = tk.Frame(root)
        self.process_fields_frame.pack(pady=10)

        # Submit and Reset Buttons
        self.submit_button = tk.Button(root, text="Submit", command=self.submit)
        self.submit_button.pack(side=tk.LEFT, padx=10)
        self.reset_button = tk.Button(root, text="Reset", command=self.reset)
        self.reset_button.pack(side=tk.RIGHT, padx=10)

        self.process_fields = []

    def update_process_fields(self, event=None):
        # Clear current fields
        # self.process_fields=[]
        for field_set in self.process_fields:
            for widget in field_set:
                widget.destroy()
        self.process_fields.clear()

        try:
            num_processes = int(self.num_processes_entry.get())
            if num_processes < 1:
                raise ValueError

            for i in range(num_processes):
                field_set = []
                for j, field in enumerate(["PID", "Arrival Time(ms)", "Burst Time(ms)"]):
                    label = tk.Label(self.process_fields_frame, text=f"{field}:")
                    label.grid(row=i, column=j*2)
                    entry = tk.Entry(self.process_fields_frame)
                    entry.grid(row=i, column=j*2 + 1)
                    field_set.append(entry)
                self.process_fields.append(field_set)

        except ValueError:
            messagebox.showerror("Invalid Input", "Number of processes should be greater than 1")

    def submit(self):
        try:
            num_processes = int(self.num_processes_entry.get())
            time_quantum = int(self.time_quantum_entry.get())
            if num_processes < 1:
                raise ValueError

            # Prepare data to write to a file
            data = []
            data.append(str(num_processes))
            for field_set in self.process_fields:
                process_data = []
                for entry in field_set:
                    process_data.append(entry.get())
                data.append(" ".join(process_data))
            data.append(str(time_quantum))

            # Write data to a file
            with open("input.txt", "w") as file:
                file.write("\n".join(data))

            # Clear previous output if any
            if os.path.exists("output.txt"):
                os.remove("output.txt")

            # Run the C++ program using subprocess
            subprocess.run(["./a.out"], check=True)

            # Read and display output from the C++ program
            with open("output.txt", "r") as file:
                output = file.read()
            self.show_output_window(output)

        except FileNotFoundError:
            messagebox.showerror("Error", "C++ executable 'a.out' not found.")
        except subprocess.CalledProcessError:
            messagebox.showerror("Execution Error", "Error occurred during execution.")
        except ValueError:
            messagebox.showerror("Invalid Input", "Please enter valid numeric values.")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def show_output_window(self, output):
        output_window = tk.Toplevel(self.root)
        output_window.title("Execution Result")

        text_box = tk.Text(output_window, wrap=tk.NONE, width=100, height=30)
        text_box.pack(expand=True, fill=tk.BOTH)
        text_box.insert(tk.END, output)
        text_box.config(state=tk.DISABLED)

        # Add horizontal and vertical scrollbars
        scrollbar_y = tk.Scrollbar(output_window, orient=tk.VERTICAL, command=text_box.yview)
        scrollbar_y.pack(side=tk.RIGHT, fill=tk.Y)
        text_box.config(yscrollcommand=scrollbar_y.set)

        scrollbar_x = tk.Scrollbar(output_window, orient=tk.HORIZONTAL, command=text_box.xview)
        scrollbar_x.pack(side=tk.BOTTOM, fill=tk.X)
        text_box.config(xscrollcommand=scrollbar_x.set)

    def reset(self):
        self.num_processes_entry.delete(0, tk.END)
        self.time_quantum_entry.delete(0, tk.END)
        self.update_process_fields()
        messagebox.showinfo("Reset", "Fields have been reset")

if __name__ == "__main__":
    root = tk.Tk()
    app = ExperimentApp(root)
    root.mainloop()
