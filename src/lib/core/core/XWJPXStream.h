/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWJPXSTREAM_H
#define XWJPXSTREAM_H

#include "XWObject.h"
#include "XWStream.h"

class JArithmeticDecoder;
class JArithmeticDecoderStats;


enum JPXColorSpaceType {
  jpxCSBiLevel = 0,
  jpxCSYCbCr1 = 1,
  jpxCSYCbCr2 = 3,
  jpxCSYCBCr3 = 4,
  jpxCSPhotoYCC = 9,
  jpxCSCMY = 11,
  jpxCSCMYK = 12,
  jpxCSYCCK = 13,
  jpxCSCIELab = 14,
  jpxCSsRGB = 16,
  jpxCSGrayscale = 17,
  jpxCSBiLevel2 = 18,
  jpxCSCIEJab = 19,
  jpxCSCISesRGB = 20,
  jpxCSROMMRGB = 21,
  jpxCSsRGBYCbCr = 22,
  jpxCSYPbPr1125 = 23,
  jpxCSYPbPr1250 = 24
};

struct JPXColorSpecCIELab 
{
    uint rl, ol, ra, oa, rb, ob, il;
};


struct JPXColorSpecEnumerated 
{
    JPXColorSpaceType type;	// color space type
    union 
    {
        JPXColorSpecCIELab cieLab;
    };
};


struct JPXColorSpec 
{
    uint meth;			// method
    int prec;			// precedence
    union 
    {
        JPXColorSpecEnumerated enumerated;
    };
};


struct JPXPalette 
{
    uint nEntries;		// number of entries in the palette
    uint nComps;			// number of components in each entry
    uint *bpc;			// bits per component, for each component
    int *c;			// color data:
				//   c[i*nComps+j] = entry i, component j
};

struct JPXCompMap 
{
    uint nChannels;		// number of channels
    uint *comp;			// codestream components mapped to each channel
    uint *type;			// 0 for direct use, 1 for palette mapping
    uint *pComp;			// palette components to use
};

struct JPXChannelDefn 
{
    uint nChannels;		// number of channels
    uint *idx;			// channel indexes
    uint *type;			// channel types
    uint *assoc;			// channel associations
};

struct JPXTagTreeNode 
{
    bool finished;		// true if this node is finished
    uint val;			// current value
};

struct JPXCodeBlock 
{
    //----- size
    uint x0, y0, x1, y1;		// bounds

    //----- persistent state
    bool seen;			// true if this code-block has already
				//   been seen
    uint lBlock;			// base number of bits used for pkt data length
    uint nextPass;		// next coding pass

    //---- info from first packet
    uint nZeroBitPlanes;		// number of zero bit planes

    //----- info for the current packet
    uint included;		// code-block inclusion in this packet:
	    			//   0=not included, 1=included
    uint nCodingPasses;		// number of coding passes in this pkt
    uint * dataLen;		// pkt data length

    uint dataLenSize;		// size of the dataLen array

  	//----- coefficient data
  	int *coeffs;
  	char *touched;		// coefficient 'touched' flags
  	ushort len;			// coefficient length
  	JArithmeticDecoder		// arithmetic decoder
    			*arithDecoder;
  	JArithmeticDecoderStats	// arithmetic decoder stats
    			*stats;
};

struct JPXSubband 
{
    //----- computed
    uint x0, y0, x1, y1;		// bounds
    uint nXCBs, nYCBs;		// number of code-blocks in the x and y
				//   directions

    //----- tag trees
    uint maxTTLevel;		// max tag tree level
    JPXTagTreeNode *inclusion;	// inclusion tag tree for each subband
    JPXTagTreeNode *zeroBitPlane;	// zero-bit plane tag tree for each
				//   subband

    //----- children
    JPXCodeBlock *cbs;		// the code-blocks (len = nXCBs * nYCBs)
};

struct JPXPrecinct 
{
    //----- computed
    uint x0, y0, x1, y1;		// bounds of the precinct

    //----- children
    JPXSubband *subbands;		// the subbands
};

struct JPXResLevel 
{
    //----- from the COD and COC segments (main and tile)
    uint precinctWidth;		// log2(precinct width)
    uint precinctHeight;		// log2(precinct height)

    //----- computed
    uint x0, y0, x1, y1;		// bounds of the tile-comp (for this res level)
    uint bx0[3], by0[3],		// subband bounds
        bx1[3], by1[3];

    //---- children
    JPXPrecinct *precincts;	// the precincts
};

struct JPXTileComp 
{
    //----- from the SIZ segment
    bool sgned;			// 1 for signed, 0 for unsigned
    uint prec;			// precision, in bits
    uint hSep;			// horizontal separation of samples
    uint vSep;			// vertical separation of samples

    //----- from the COD and COC segments (main and tile)
    uint style;			// coding style parameter (Scod / Scoc)
    uint nDecompLevels;		// number of decomposition levels
    uint codeBlockW;		// log2(code-block width)
    uint codeBlockH;		// log2(code-block height)
    uint codeBlockStyle;		// code-block style
    uint transform;		// wavelet transformation

    //----- from the QCD and QCC segments (main and tile)
    uint quantStyle;		// quantization style
    uint *quantSteps;		// quantization step size for each subband
    uint nQuantSteps;		// number of entries in quantSteps

    //----- computed
    uint x0, y0, x1, y1;		// bounds of the tile-comp, in ref coords
    uint w;
    uint cbW;			// code-block width
    uint cbH;			// code-block height

    //----- image data
    int *data;			// the decoded image data
    int *buf;			// intermediate buffer for the inverse
				//   transform

    //----- children
    JPXResLevel *resLevels;	// the resolution levels
				//   (len = nDecompLevels + 1)
};

struct JPXTile 
{
  bool init;

  //----- from the COD segments (main and tile)
  uint progOrder;		// progression order
  uint nLayers;		// number of layers
  uint multiComp;		// multiple component transformation

  //----- computed
  uint x0, y0, x1, y1;		// bounds of the tile, in ref coords
  uint maxNDecompLevels;	// max number of decomposition levels used
				//   in any component in this tile

  //----- progression order loop counters
  uint comp;			//   component
  uint res;			//   resolution level
  uint precinct;		//   precinct
  uint layer;			//   layer

  //----- children
  JPXTileComp *tileComps;	// the tile-components (len = JPXImage.nComps)
};

struct JPXImage 
{
    //----- from the SIZ segment
    uint xSize, ySize;		// size of reference grid
    uint xOffset, yOffset;	// image offset
    uint xTileSize, yTileSize;	// size of tiles
    uint xTileOffset,		// offset of first tile
           yTileOffset;
    uint nComps;			// number of components

    //----- computed
    uint nXTiles;		// number of tiles in x direction
    uint nYTiles;		// number of tiles in y direction

    //----- children
    JPXTile *tiles;		// the tiles (len = nXTiles * nYTiles)
};

class XW_CORE_EXPORT XWJPXStream : public XWFilterStream
{
public:
    XWJPXStream(XWStream *strA);
    virtual ~XWJPXStream();
    
    virtual void close();
    
    virtual int  getChar();
    virtual void getImageParams(int *bitsPerComponent, int *csMode);
    virtual int  getKind() { return STREAM_JPX; }
    virtual XWString *getPSFilter(int, const char *);
    
    virtual bool isBinary(bool);
    
    virtual int lookChar();    
			      
    virtual void reset();
    
private:
    void  fillReadBuf();
    uint finishBitBuf();
    
    void getImageParams2(int *bitsPerComponent, int *csMode);
    
    bool inverseMultiCompAndDC(JPXTile *tile);
    void inverseTransform(JPXTileComp *tileComp);
    void inverseTransformLevel(JPXTileComp *tileComp,
			                   uint r, 
			                   JPXResLevel *resLevel);
	void inverseTransform1D(JPXTileComp *tileComp, 
	                        int *data,
				                  uint offset, 
				                  uint n);
    
    bool readBits(int nBits, uint *x);
    bool readBoxes();
    bool readBoxHdr(uint *boxType, uint *boxLen, uint *dataLen);
    bool readByte(int *x);
    bool readCodeBlockData(JPXTileComp *tileComp,
			               JPXResLevel *,
			               JPXPrecinct *,
			               JPXSubband *,
			               uint res, 
			               uint sb,
			               JPXCodeBlock *cb);
    bool readCodestream(uint len);
    bool readColorSpecBox(uint dataLen);
    int  readMarkerHdr(int *segType, uint *segLen);
    bool readNBytes(int nBytes, bool signd, int *x);
    bool readTilePart();
    bool readTilePartData(uint tileIdx, uint tilePartLen, bool tilePartToEOC);
    bool readUByte(uint *x);
    bool readULong(uint *x);
    bool readUWord(uint *x);
    
    void skipEPH();
    void skipSOP();
    void startBitBuf(uint byteCountA);
    
private:
	  XWBufStream *bufStr;		// buffered stream (for lookahead)
    uint nComps;			// number of components
    uint *bpc;			// bits per component, for each component
    uint width, height;		// image size
    bool haveImgHdr;		// set if a JP2/JPX image header has been
				//   found
    JPXColorSpec cs;		// color specification
    bool haveCS;			// set if a color spec has been found
    JPXPalette palette;		// the palette
    bool havePalette;		// set if a palette has been found
    JPXCompMap compMap;		// the component mapping
    bool haveCompMap;		// set if a component mapping has been found
    JPXChannelDefn channelDefn;	// channel definition
    bool haveChannelDefn;	// set if a channel defn has been found

    JPXImage img;			// JPEG2000 decoder data
    uint bitBuf;			// buffer for bit reads
    int bitBufLen;		// number of bits in bitBuf
    bool bitBufSkip;		// true if next bit should be skipped
				//   (for bit stuffing)
    uint byteCount;		// number of available bytes left

    uint curX, curY, curComp;	// current position for lookChar/getChar
    uint readBuf;		// read buffer
    uint readBufLen;		// number of valid bits in readBuf
};

#endif // XWJPXSTREAM_H

