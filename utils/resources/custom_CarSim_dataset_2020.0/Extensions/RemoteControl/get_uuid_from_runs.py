import os

list_txt = "uuid_list.txt"
text_list = []
curr_dir = os.getcwd()
folder_results_path = os.path.join(curr_dir , "Runs")

run_list = os.listdir(folder_results_path)
list_results = open(list_txt, "w") 
for item in run_list:
    if ("Run" in str(item)) and ("par" in str(item)):
        list_results.write(item)
        list_results.write("\n")

list_results.close()

with open(list_txt, 'r') as f:
    for line in f:
        text_list.append(line)


title_txt = "category_title_uuid_from_runs_folder.txt"
results_file = open(title_txt, 'w')

for text in text_list:
    curr_category = ""
    curr_title = ""
    run_name = text.strip()
    curr_file = os.path.join(folder_results_path, run_name)
    if os.path.isfile(curr_file):
        with open(curr_file, 'r') as curr:
            for line1 in curr:
                if "#FullDataName" in line1:
                    curr_category = line1
    
    results_file.write('%-42s' % str(text.split('.')[0]))
    #results_file.write('%-53s' % str(curr_category.rstrip('\n')))
    #results_file.write('%-53s' % str(curr_category.split('`')[2].rstrip('\n')))
    results_file.write('%-53s' % str(curr_category.split('`')[2].rstrip()))
    results_file.write('%-53s' % str(curr_category.split('`')[1]))
    #results_file.write('%-30s' % str(curr_title).rstrip('\n'))
    results_file.write('\n')

results_file.close()

