#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QVector>
#include <QPushButton>
#include <QGraphicsRectItem>

#include "globaldatatransition.h"

namespace Ui {
class Graphic;
}

class Graphic : public QWidget
{
    Q_OBJECT

public:
    explicit Graphic(QWidget *parent = nullptr, GlobalDataTransition* data = nullptr);
    ~Graphic();

private:
    struct Lesson
    {
        QString group;
        QString day;
        QString time;
        QString subject;
        QString room;
        QString teacher;
    };
    GlobalDataTransition* data;

    void init_response_api(QPushButton *but_api);
    void clearScenes();
    void updateStatus(const QString& text, bool isError = false);
    bool parseSchedule(const QJsonObject& obj, QVector<Lesson>& lessons);
    QStringList orderedDays(const QVector<Lesson>& lessons) const;
    QStringList orderedTimes(const QVector<Lesson>& lessons) const;
    void renderAll(const QVector<Lesson>& lessons);
    void renderDiagram(QGraphicsScene* scene,
                       QGraphicsView* view,
                       const QVector<Lesson>& lessons,
                       const QStringList& days,
                       const QStringList& times,
                       const std::function<QString(const Lesson&)>& rowSelector,
                       const QString& emptyText);

    Ui::Graphic *ui;
    QGraphicsScene* sceneGroups;
    QGraphicsScene* sceneTeachers;
    QGraphicsScene* sceneRooms;
};

namespace {
class HoverRectItem : public QGraphicsRectItem
{
public:
    HoverRectItem(const QRectF& rect, const QColor& color, const QString& tooltip)
        : QGraphicsRectItem(rect), baseColor(color)
    {
        setBrush(baseColor);
        setPen(QPen(Qt::black, 1));
        setAcceptHoverEvents(true);
        setToolTip(tooltip);
    }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent*) override { setBrush(baseColor.lighter(130)); }
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override { setBrush(baseColor); }

private:
    QColor baseColor;
};

QString shortText(const QString& src, int maxLen = 10)
{
    if (src.size() <= maxLen) {
        return src;
    }
    return src.left(maxLen - 1) + QStringLiteral("…");
}

QString compactSubjectText(const QString& src, int maxLen = 10)
{
    QString normalized = src.trimmed();
    if (normalized.isEmpty()) {
        return normalized;
    }

    // Сжимаем подписи в скобках: "(лекция)" -> "(л)", "(лабораторная работа)" -> "(л)"
    int openPos = normalized.indexOf('(');
    while (openPos != -1) {
        const int closePos = normalized.indexOf(')', openPos + 1);
        if (closePos == -1) {
            break;
        }

        const QString inside = normalized.mid(openPos + 1, closePos - openPos - 1).trimmed();
        if (!inside.isEmpty()) {
            const QString firstLetter = inside.left(1).toLower();
            normalized.replace(openPos, closePos - openPos + 1, "(" + firstLetter + ")");
            openPos = normalized.indexOf('(', openPos + 3);
        } else {
            openPos = normalized.indexOf('(', closePos + 1);
        }
    }

    const QString trimmed = normalized;
    if (trimmed.isEmpty()) {
        return trimmed;
    }

    if (trimmed.size() <= maxLen) {
        return trimmed;
    }

    // Для длинных многословных названий: аббревиатура из первых букв слов.
    // Например: "Методы оптимизации и анализа систем" -> "МОИАС".
    const QStringList words = trimmed.simplified().split(' ', Qt::SkipEmptyParts);
    if (words.size() >= 2) {
        QString abbr;
        for (const QString& w : words) {
            if (!w.isEmpty()) {
                abbr += w.at(0).toUpper();
            }
        }
        if (!abbr.isEmpty()) {
            if (abbr.size() > maxLen) {
                return abbr.left(maxLen);
            }
            return abbr;
        }
    }

    return shortText(trimmed, maxLen);
}
} // namespace

#endif // GRAPHIC_H
