workspace {
    name "Pillow"
    description "Система управления недвижимостью"

    model {
        # Акторы (роли пользователей)
        guest = person "Гость" "Незарегистрированный пользователь, ищет объекты недвижимости"
        user = person "Зарегистрированный пользователь" "Может добавлять объекты, записываться на просмотры, управлять своими объектами"
        admin = person "Администратор" "Управляет системой"

        # Внешние системы
        emailService = softwareSystem "Email Service" "Сервис отправки email-уведомлений" "external"

        # Внутренняя система
        realEstateSystem = softwareSystem name {
            description description
            
            # Связи с внешним миром
            guest -> this "Поиск объектов по городу/цене"
            user -> this "Управление объектами и просмотрами"
            admin -> this "Управление системой"
            this -> emailService "Отправка уведомлений" "SMTP"

            # КОНТЕЙНЕРЫ
            # Веб-приложение
            webApp = container "Web Application" "Веб-интерфейс для пользователей" "React" "web"

            # API Gateway
            apiGateway = container "API Gateway" "Шлюз для маршрутизации запросов" "Node.js Express"

            # Микросервисы
            userService = container "User Service" "Управление пользователями: регистрация, поиск по логину, по маске имени/фамилии" "Python FastAPI"
            propertyService = container "Property Service" "Управление объектами недвижимости: добавление, поиск по городу/цене, изменение статуса" "Java Spring Boot"
            viewingService = container "Viewing Service" "Управление просмотрами: запись на просмотр, получение записей" "Go"

            # База данных (для простоты я взял одну общую)
            database = container "Database" "Хранение всех данных системы" "PostgreSQL" "database"

            # Связи между контейнерами
            webApp -> apiGateway "Все запросы к API" "HTTPS/REST"

            apiGateway -> userService "Маршрутизация /api/users/*" "HTTPS/REST"
            apiGateway -> propertyService "Маршрутизация /api/properties/*" "HTTPS/REST"
            apiGateway -> viewingService "Маршрутизация /api/viewings/*" "HTTPS/REST"

            userService -> database "CRUD пользователей" "PostgreSQL"
            propertyService -> database "CRUD объектов" "PostgreSQL"
            viewingService -> database "CRUD просмотров" "PostgreSQL"

            # Взаимодействие между сервисами
            viewingService -> propertyService "Проверка существования объекта" "HTTPS/REST"
            viewingService -> userService "Проверка существования пользователя" "HTTPS/REST"

            # Обратные связи
            propertyService -> viewingService "Возвращает информацию об объекте" "HTTPS/REST"
            userService -> viewingService "Возвращает информацию о пользователе" "HTTPS/REST"
            database -> viewingService "Подтверждает создание записи" "PostgreSQL"
            
            viewingService -> apiGateway "Возвращает данные" "HTTPS/REST"
            apiGateway -> webApp "Возвращает ответ" "HTTPS/REST"
            
            webApp -> guest "Отображает результаты поиска" "HTTPS"
            webApp -> user "Отображает результаты операций" "HTTPS"
            webApp -> admin "Отображает панель управления" "HTTPS"

            # Связи ролей с контейнерами
            guest -> webApp "Использует для поиска" "HTTPS"
            user -> webApp "Использует для работы с системой" "HTTPS"
            admin -> webApp "Использует для администрирования" "HTTPS"
        }
    }

    views {
        # Диаграмма контекста системы (C1)
        systemContext realEstateSystem "system_context" {
            title "System Context diagram - Pillow"
            include *
            autoLayout
        }

        # Диаграмма контейнеров (C2)
        container realEstateSystem "containers" {
            title "Container diagram - Pillow"
            include *
            autoLayout
        }


        # Возьмем сценарий: Пользователь хочет записаться на просмотр квартиры
        # Динамическая диаграмма
        dynamic realEstateSystem "create_viewing" {
            title "Сценарий: Запись пользователя на просмотр объекта"
            
            user -> webApp "1. Нажимает 'Записаться на просмотр'"
            webApp -> apiGateway "2. POST /api/viewings"
            apiGateway -> viewingService "3. Передает запрос"
            
            viewingService -> propertyService "4. Проверяет существование объекта"
            propertyService -> viewingService "5. Объект существует"
            
            viewingService -> userService "6. Проверяет существование пользователя"
            userService -> viewingService "7. Пользователь существует"
            
            viewingService -> database "8. Сохраняет запись"
            database -> viewingService "9. Подтверждает сохранение"
            
            viewingService -> apiGateway "10. Возвращает данные"
            apiGateway -> webApp "11. Возвращает ответ"
            webApp -> user "12. Показывает подтверждение"
            
            autoLayout lr
        }

        styles {
            element "external" {
                background #e0e0e0
                color #555555
                shape roundedBox
            }

            relationship "external" {
                color #999999
                dashed true
            }

            element "database" {
                shape Cylinder
                background #438dd5
                color #ffffff
            }

            element "web" {
                shape WebBrowser
                background #438dd5
                color #ffffff
            }
            
            element "Person" {
                shape Person
                background #08427b
                color #ffffff
            }
        }

        themes default
    }
}