# SPbAU-OS-Course
SPbAU-OS-Course (Term 4)

# Файловая система
Файловая система - это дерево fs_node-ов.

fs_node - имя файла, файл и список файлов, которые находятся в этой ноде (если это директория).

Файл - ссылка на данные, размер данных, тип, лок.
Данные файла хранятся в одним куском.

# Файлы
* fs.h, fs.c - файловая система.
* initramfs.h, initramfs.c - загрузка образа.
