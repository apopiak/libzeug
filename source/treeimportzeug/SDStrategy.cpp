#include <treeimportzeug/SDStrategy.h>

#include <QDateTime>
#include <QSqlRecord>

#include <treezeug/Tree.h>

namespace zeug
{

SDStrategy::SDStrategy()
: m_tree(new Tree(""))
{
}

void SDStrategy::createTrees()
{
    loadDirectories();
    loadFiles();
    insertNodesIntoTree();
    addMetrics();
}

void SDStrategy::clear()
{
    delete m_tree;

    m_tree = nullptr;
    m_nodes.clear();
    m_parents.clear();
    m_metrics.clear();
}

QSet<QString> SDStrategy::wantedFileSuffixes() const
{
    return QSet<QString>() << "sqlite" << "db";
}

bool SDStrategy::wantsToProcess(QSqlDatabase& database) const
{
    return database.record("nodes").isEmpty();
}

void SDStrategy::loadDirectories()
{
	for (const QVariantMap& directory : executeQuery("SELECT id, parent, name FROM directories WHERE 1 ORDER BY id"))
	{
		Node* node = new Node(directory["id"].toInt());
		node->setName(directory["name"].toString().toStdString());
		
		m_nodes[directory["id"].toInt()] = node;
		m_parents[directory["id"].toInt()] = directory["parent"].toInt();
	}
}

void SDStrategy::loadFiles()
{
	for (const QVariantMap& file : executeQuery("SELECT id, directory, name FROM items WHERE 1 ORDER BY id"))
	{
		Node* node = new Node(file["id"].toInt());
		node->setName(file["name"].toString().toStdString());
		
		m_nodes[file["id"].toInt()] = node;
		m_parents[file["id"].toInt()] = file["directory"].toInt();
	}
}

void SDStrategy::insertNodesIntoTree()
{
	for (Node* node : m_nodes)
	{
		insertIntoTree(node);
	}
}

void SDStrategy::insertIntoTree(Node* node)
{
	if (m_tree->getNode(node->id()))
	{
		return;
	}
	
	if (!m_tree->getNode(m_parents.value(node->id())))
	{
		insertIntoTree(m_nodes.value(m_parents.value(node->id())));
	}
	
	m_tree->getNode(m_parents.value(node->id()))->addChild(node);
}

void SDStrategy::addMetrics()
{
	m_tree->addAttributeMap("id", AttributeMap::Numeric);
	m_tree->addAttributeMap("depth", AttributeMap::Numeric);
	
	m_tree->nodesDo([](Node* node) {
		node->setAttribute("id", node->id());
		node->setAttribute("depth", node->depth());
	});
	
	for (const QVariantMap& metricMeta : executeQuery("SELECT id, name, type FROM metricsmeta WHERE 1 ORDER BY id"))
	{
		m_tree->addAttributeMap(
			metricMeta["name"].toString().toStdString(),
			metricMeta["type"].toInt() == 1 ? AttributeMap::Numeric : AttributeMap::Nominal
		);
		
		m_metrics[metricMeta["id"].toInt()] = metricMeta["name"].toString();
	}
	
	for (const QVariantMap& metricSet : executeQuery("SELECT timestamp FROM metricssets WHERE 1 ORDER BY timestamp"))
	{
		Tree* tree = m_tree->copy();

        QDateTime time;
        time.setMSecsSinceEpoch(metricSet["timestamp"].toULongLong());

        m_tree->setName(time.toString("dd.MM.yyyy hh:mm").toStdString());
		
		for (const QVariantMap& metricData : executeQuery("SELECT itemId, metricId, value FROM metricsdata WHERE timestamp = " + metricSet["timestamp"].toString() + " ORDER BY itemId"))
		{
			tree->getNode(metricData["itemId"].toInt())->setAttribute(
				m_metrics[metricData["metricId"].toInt()].toStdString(),
				metricData["value"].toString().toStdString()
			);
		}
		
		m_trees << tree;
	}
	
	delete m_tree;
}

} // namespace zeug
