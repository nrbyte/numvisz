#include "visualizationsdao.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>

#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>


VisualizationsDAO::VisualizationsDAO(QObject *parent)
    : QObject{parent}
{
    // Check the connection hasn't already been made
    QSqlDatabase db = QSqlDatabase::database("visualizations");
    if (db.isValid()) {
        return;
    }
    // Create and open the database connection
    db = QSqlDatabase::addDatabase("QSQLITE", "visualizations");
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    db.setDatabaseName(dbPath + "/data.db");
    if (!db.open())
    {
        QMessageBox errorBox;
        errorBox.setText("Failed to open database");
        errorBox.setWindowTitle("Error");
        errorBox.exec();
        std::exit(-1);
    }

    // Create (if necessary) the table
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS Visualizations("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name TEXT NOT NULL,"
               "csvPath TEXT NOT NULL,"
               "fontPath TEXT NOT NULL,"
               "barHeight INTEGER NOT NULL);");
}

QString VisualizationsDAO::getName(int index)
{
    // Query the database
    QSqlQuery query(QSqlDatabase::database("visualizations"));
    query.prepare("SELECT name FROM Visualizations WHERE id == :index");
    query.bindValue(":index", index + 1);
    query.exec();

    // Fetch the first row only (the ID's are unique)
    query.next();

    return query.value(0).toString();
}

int VisualizationsDAO::getRowCount()
{
    QSqlDatabase db = QSqlDatabase::database("visualizations");

    QSqlQuery query(db);
    query.exec("SELECT id FROM Visualizations;");

    if (db.driver()->hasFeature(QSqlDriver::QuerySize))
    {
        return query.size();
    } else {
        // If the table is empty, last() will fail so return 0
        if (!query.last()) {
            return 0;
        }
        return query.at() + 1;
    }
}


