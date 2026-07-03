#include "../../include/httpResponse/MimeTypes.hpp"

std::map<std::string, std::string> MimeTypes::_types;

MimeTypes::MimeTypes()
{}

MimeTypes::~MimeTypes()
{}

void MimeTypes::_init()
{
    // Remplissage de la map
    if (!_types.empty())
        return;

    // Texte / Code
    _types["html"] = "text/html; charset=utf-8";
    _types["htm"]  = "text/html; charset=utf-8";
    _types["css"]  = "text/css";
    _types["js"]   = "application/javascript";
    _types["txt"]  = "text/plain; charset=utf-8";
    _types["xml"]  = "application/xml";
    _types["json"] = "application/json";

    // Images
    _types["png"]  = "image/png";
    _types["jpg"]  = "image/jpeg";
    _types["jpeg"] = "image/jpeg";
    _types["gif"]  = "image/gif";
    _types["ico"]  = "image/x-icon";
    _types["svg"]  = "image/svg+xml";
    _types["bmp"]  = "image/bmp";
    _types["webp"] = "image/webp";

    // Documents / Autres
    _types["pdf"]  = "application/pdf";
    _types["zip"]  = "application/zip";
    _types["mp4"]  = "video/mp4";
    _types["mp3"]  = "audio/mpeg";
}

std::string MimeTypes::getType(const std::string& path)
{
    if (_types.empty()) 
        _init();

    // Trouver l'extension
    size_t dotPos = path.rfind('.');

    if (dotPos == std::string::npos || dotPos == path.length() - 1) 
        return "application/octet-stream";

    // Extraire l'extension (après le point)
    std::string ext = path.substr(dotPos + 1);

    // Chercher dans la map
    std::map<std::string, std::string>::iterator it = _types.find(ext);

    if (it != _types.end()) 
        return it->second; // Retourne le type trouvé (ex: "text/html")

    // Type par défaut pour les fichiers inconnus (téléchargement binaire)
    return "application/octet-stream";
}