#ifndef H__dfsch_lib__inet__
#define H__dfsch_lib__inet__

#include <dfsch/dfsch.h>
#include <dfsch/strings.h>

dfsch_object_t* dfsch_module_inet_register(dfsch_object_t* env);

dfsch_object_t* dfsch_http_split_path(char* path);


dfsch_object_t* dfsch_http_query_2_hash(char* query);
dfsch_object_t* dfsch_http_query_2_alist(char* query);

dfsch_strbuf_t* dfsch_inet_urldecode(dfsch_strbuf_t* str_buf);
dfsch_strbuf_t* dfsch_inet_urlencode(dfsch_strbuf_t* str_buf);

dfsch_strbuf_t* dfsch_inet_base64_decode(dfsch_strbuf_t* str_buf);
dfsch_strbuf_t* dfsch_inet_base64_encode(dfsch_strbuf_t* str_buf,
                                         int wrap,
                                         int pad);
dfsch_strbuf_t* dfsch_inet_uri_base64_decode(dfsch_strbuf_t* str_buf);
dfsch_strbuf_t* dfsch_inet_uri_base64_encode(dfsch_strbuf_t* str_buf);


#endif
