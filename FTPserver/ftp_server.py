from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler
from pyftpdlib.servers import FTPServer


class MyHandler(FTPHandler):

    def on_connect(self):
        print("%s:%s connected" % (self.remote_ip, self.remote_port))

    def on_disconnect(self):
        print("%s:%s disconnected" % (self.remote_ip, self.remote_port))
        pass

    def on_login(self, username):
        pass

    def on_logout(self, username):
        print("user %s logged out" % self.username)
        pass

    def on_file_sent(self, file):
        print("file sent successfully", file)
        pass

    def on_file_received(self, file):
        print("file received successfully ", file)
        pass

    def on_incomplete_file_sent(self, file):
        print("incomplete file sent", file)
        pass

    def on_incomplete_file_received(self, file):
        print("incomplete file received", file)
        import os
        os.remove(file)


def main():
    # Добавление и авторизация пользователей: с полным разрешением, с частичным разрешением и анонимный read-only
    authorizer = DummyAuthorizer()
    authorizer.add_user('peter', '12345', homedir='server_catalog', perm='elradfmwMT', msg_login='Login successful. '
                                                                                                 'Welcome to FTP '
                                                                                                 'server',
                        msg_quit='221 Goodbye.')

    authorizer.add_user('kate', 'abc', homedir='server_catalog', perm='elr')
    authorizer.override_perm('kate', 'server_catalog/folder_1', 'lr', recursive=False)
    authorizer.override_perm('kate', 'server_catalog/client_files', 'elrw', recursive=False)

    authorizer.add_anonymous(homedir='server_catalog/client_files')

    # Обработчик событий
    handler = MyHandler
    handler.authorizer = authorizer
    handler.use_sendfile = True

    authorizer.validate_authentication('peter', '12345', handler)

    # Определение баннера (сообщения пользователю при подключении)
    handler.banner = "pyftpdlib based ftpd ready."

    # Опредление портов, которые можно использовать для пассивного режима
    handler.passive_ports = range(60000, 65535)

    # Создание сервера, слушающего по адресу 127.0.0.1:2121
    address = ('127.0.0.1', 2121)
    server = FTPServer(address, handler)
    # Установление ограничения на количество подключений
    server.max_cons = 256
    server.max_cons_per_ip = 5
    # старт ftp сервера
    server.serve_forever()

    # Закрытие всех соединений
    server.close_all()


if __name__ == '__main__':
    main()
