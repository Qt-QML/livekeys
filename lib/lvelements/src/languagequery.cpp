#include "languagequery.h"
#include "live/visuallog.h"
#include "tree_sitter/api.h"

namespace lv{ namespace el{

// LanguageQueryException
// -----------------------------------------------------------------------------

LanguageQueryException::LanguageQueryException(const std::string &message, uint32_t offset, lv::Exception::Code code)
    : Exception(message, code)
    , m_offset(offset)
{
}

// LanguageQuery::Cursor
// -----------------------------------------------------------------------------

bool LanguageQuery::Cursor::nextMatch(){
    TSQueryCursor* cursor = reinterpret_cast<TSQueryCursor*>(m_cursor);
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    return ts_query_cursor_next_match(cursor, currentMatch);
}

uint16_t LanguageQuery::Cursor::totalMatchCaptures() const{
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    return currentMatch->capture_count;
}

uint16_t LanguageQuery::Cursor::matchPatternIndex() const{
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    return currentMatch->pattern_index;
}

SourceRange LanguageQuery::Cursor::captureRange(uint16_t captureIndex){
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    TSNode node = currentMatch->captures[captureIndex].node;

    uint32_t start = ts_node_start_byte(node);
    uint32_t end   = ts_node_end_byte(node);

    return SourceRange(start, end - start);
}

uint32_t LanguageQuery::Cursor::captureId(uint16_t captureIndex){
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    return currentMatch->captures[captureIndex].index;
}

LanguageQuery::Cursor::Cursor()
    : m_cursor(ts_query_cursor_new())
    , m_currentMatch(new TSQueryMatch)
{
}

LanguageQuery::Cursor::~Cursor(){
    TSQueryCursor* cursor = reinterpret_cast<TSQueryCursor*>(m_cursor);
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    ts_query_cursor_delete(cursor);
    delete currentMatch;
}


// LanguageQuery
// -----------------------------------------------------------------------------

LanguageQuery::Ptr LanguageQuery::create(const Parser &parser, const std::string &queryString){
    uint32_t errorOffset;
    TSQueryError errorType;

    TSQuery * query = ts_query_new(
      ts_parser_language(parser.internal()),
      queryString.c_str(),
      static_cast<uint32_t>(queryString.size()),
      &errorOffset,
      &errorType
    );

    if ( errorType != TSQueryErrorNone ){
        ts_query_delete(query);
        THROW_EXCEPTION(lv::el::LanguageQueryException, "Language query error.", errorType);
    }

    return LanguageQuery::Ptr(new LanguageQuery(static_cast<void*>(query)));
}

LanguageQuery::~LanguageQuery(){
    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);
    ts_query_delete(query);
}

uint32_t LanguageQuery::captureCount() const{
    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);
    return ts_query_capture_count(query);
}

std::string LanguageQuery::captureName(uint32_t captureIndex) const{
    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);

    uint32_t captureLength;
    const char* captureName = ts_query_capture_name_for_id(query, captureIndex, &captureLength);
    return std::string(captureName, captureLength);
}

LanguageQuery::Cursor::Ptr LanguageQuery::exec(Parser::AST *ast){
    TSTree* tree = reinterpret_cast<TSTree*>(ast);
    TSNode root = ts_tree_root_node(tree);

    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);

    Cursor::Ptr cursor(new Cursor);
    TSQueryCursor* cursorInternal = reinterpret_cast<TSQueryCursor*>(cursor->m_cursor);

    ts_query_cursor_exec(cursorInternal, query, root);
    return cursor;
}

LanguageQuery::Cursor::Ptr LanguageQuery::exec(Parser::AST *ast, uint32_t start, uint32_t end){
    TSTree* tree = reinterpret_cast<TSTree*>(ast);
    TSNode root = ts_tree_root_node(tree);

    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);

    Cursor::Ptr cursor(new Cursor);
    TSQueryCursor* cursorInternal = reinterpret_cast<TSQueryCursor*>(cursor->m_cursor);

    ts_query_cursor_set_byte_range(cursorInternal, start, end);
    ts_query_cursor_exec(cursorInternal, query, root);

    return cursor;
}

LanguageQuery::LanguageQuery(void *query)
    : m_query(query)
{
}

}}// namespace lv, el