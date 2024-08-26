#include "visualizationsdao.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlError>

#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

#include <iostream>

VisualizationsDao::VisualizationsDao(QObject *parent)
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
               "barHeight INTEGER NOT NULL,"
               "timePerCategory INTEGER NOT NULL);");
}

QString VisualizationsDao::getName(int index)
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

int VisualizationsDao::getRowCount()
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

void VisualizationsDao::addEntry(const QString &name, const QString &csvPath, const QString& fontPath)
{
    QSqlQuery query(QSqlDatabase::database("visualizations"));
    query.prepare("INSERT INTO Visualizations (name, csvPath, fontPath, barHeight, timePerCategory) "
                  "VALUES (:name, :csvPath, :fontPath, 35, 1000);");

    query.bindValue(":name", name);
    query.bindValue(":csvPath", csvPath);
    query.bindValue(":fontPath", fontPath);

    if (!query.exec()) {
        std::cerr << query.lastError().driverText().toStdString()
                  << ", " << query.lastError().databaseText().toStdString()
                  << std::endl;
        return;
    }
}

void VisualizationsDao::updateEntry(VisualizationEntry &entry)
{
    QSqlQuery query(QSqlDatabase::database("visualizations"));
    query.prepare("UPDATE Visualizations "
                  "SET name = :name, fontPath = :fontPath, barHeight = :barHeight, timePerCategory = :tPC "
                  "WHERE id = :id;");
    query.bindValue(":name", entry.name);
    query.bindValue(":fontPath", entry.fontPath);
    query.bindValue(":barHeight", entry.barHeight);
    query.bindValue(":tPC", entry.timePerCategory);
    query.bindValue(":id", entry.id);

    query.exec();
}

VisualizationEntry VisualizationsDao::getEntry(int id)
{
    QSqlQuery query(QSqlDatabase::database("visualizations"));
    query.prepare("SELECT id, name, csvPath, fontPath, barHeight, timePerCategory FROM Visualizations WHERE id = :id");
    query.bindValue(":id", id);
    query.exec();

    // Go to the first result
    query.next();
    VisualizationEntry entry = {
        query.value(0).toInt(),
        query.value(1).toString(),
        query.value(2).toString(),
        query.value(3).toString(),
        query.value(4).toInt(),
        query.value(5).toInt()
    };
    return entry;
}


