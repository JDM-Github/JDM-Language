#pragma once

#define JDM_DLL            __declspec(dllexport)
#define EndLine            ';'
#define FileStream         const std::string
#define InputStream        const std::string
#define Log                std::cout

#define CVoid              const void
#define CBool              const bool

#define SizeT              size_t
#define CSizeT             const size_t
#define CTokenType         const JDM::TokenType

#define StaticCVoid        static const void
#define StaticCInt         static const int
#define StaticCBool        static const bool

#define SpaceString        const std::string&
#define CStringRef         const std::string&
#define CString            const std::string
#define StringStd          std::string

#define TokenStr           std::string
#define CTokenStrRef       const TokenStr&

#define VecChar            std::vector<char>
#define CVecChar           const VecChar
#define CVecCharRef        const VecChar&

#define VecTokenStr        std::vector<TokenStr>
#define CVecTokenStr       const VecTokenStr
#define CVecTokenStrRef    const VecTokenStr&

#define MapTokens          std::unordered_map<TokenStr, JDM::TokenType>
#define CMapTokens         const MapTokens

#define SharedTokenStruct     std::shared_ptr<TokenStruct>
#define MSharedTokenStruct    std::make_shared<TokenStruct>
#define CSharedTokenStruct    const SharedTokenStruct
#define CSharedTokenStructRef const SharedTokenStruct&

#define SharedTokenLink       std::shared_ptr<TokenCurrentLink>
#define MSharedTokenLink      std::make_shared<TokenCurrentLink>
#define CSharedTokenLink      const SharedTokenStruct
#define CSharedTokenLinkRef   const SharedTokenStruct&
