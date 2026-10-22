/*
 * SPDX-FileCopyrightText: 2026 SonicDE Community
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QIcon>
#include <QTemporaryDir>
#include <QTest>

#include <KIconTheme>

class KIconThemeInitializationTest : public QObject {
    Q_OBJECT
public:
    QString preferredPath;
    QString systemPath;
    QString expectedTheme;

private Q_SLOTS:
    void preservesExplicitPluginPrecedence()
    {
        const auto paths = QCoreApplication::libraryPaths();
        const int preferred = paths.indexOf(preferredPath + QStringLiteral("/kiconthemes6"));
        const int system = paths.indexOf(systemPath + QStringLiteral("/kiconthemes6"));
        QVERIFY2(preferred >= 0, qPrintable(paths.join(QLatin1Char(':'))));
        QVERIFY(system >= 0);
        QVERIFY2(preferred < system, qPrintable(paths.join(QLatin1Char(':'))));
    }

    void defaultAndUserThemeSelection()
    {
        QCOMPARE(QIcon::themeName(), QStringLiteral("KIconEngine"));
        QCOMPARE(KIconTheme::current(), expectedTheme);
    }
};

int main(int argc, char** argv)
{
    QTemporaryDir fixture;
    if (!fixture.isValid()) {
        return 1;
    }
    const QString root = fixture.path();
    const QString config = root + QStringLiteral("/config");
    const QString preferred = root + QStringLiteral("/preferred");
    const QString system = root + QStringLiteral("/system");
    if (!QDir().mkpath(config) || !QDir().mkpath(preferred + QStringLiteral("/kiconthemes6")) || !QDir().mkpath(system + QStringLiteral("/kiconthemes6"))) {
        return 1;
    }
    const QString configured = qEnvironmentVariable("KICONTHEMES_TEST_THEME");
    if (!configured.isEmpty()) {
        QFile file(config + QStringLiteral("/kdeglobals"));
        if (!file.open(QIODevice::WriteOnly) || file.write((QStringLiteral("[Icons]\nTheme=%1\n").arg(configured)).toUtf8()) < 0) {
            return 1;
        }
    }
    qputenv("HOME", root.toUtf8());
    qputenv("XDG_CONFIG_HOME", config.toUtf8());
    qputenv("XDG_CONFIG_DIRS", (root + QStringLiteral("/empty-config")).toUtf8());
    qputenv("XDG_DATA_HOME", (root + QStringLiteral("/data")).toUtf8());
    qputenv("XDG_DATA_DIRS", (root + QStringLiteral("/empty-data")).toUtf8());
    qputenv("XDG_CACHE_HOME", (root + QStringLiteral("/cache")).toUtf8());
    qputenv("QT_QPA_PLATFORMTHEME", "generic");
    QCoreApplication::setLibraryPaths(QStringList{preferred, system} + QCoreApplication::libraryPaths());
    KIconTheme::initTheme();
    QGuiApplication app(argc, argv);
    KIconThemeInitializationTest test;
    test.preferredPath = preferred;
    test.systemPath = system;
    test.expectedTheme = configured.isEmpty() ? QStringLiteral("silver") : configured;
    return QTest::qExec(&test, argc, argv);
}

#include "kiconthemeinitialization_unittest.moc"
