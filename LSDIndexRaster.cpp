//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSDIndexRaster
// Land Surface Dynamics IndexRaster
//
// An object within the University
//  of Edinburgh Land Surface Dynamics group topographic toolbox
//  for manipulating
//  and analysing raster data, with a particular focus on topography
//
// The IndexRaster object stores only integer values and is used mostly
//  for storing indices into raster data.
//
// Developed by:
//  Simon M. Mudd
//  Martin D. Hurst
//  David T. Milodowski
//  Stuart W.D. Grieve
//  Declan A. Valters
//  Fiona Clubb
//
// Copyright (C) 2013 Simon M. Mudd 2013
//
// Developer can be contacted by simon.m.mudd _at_ ed.ac.uk
//
//    Simon Mudd
//    University of Edinburgh
//    School of GeoSciences
//    Drummond Street
//    Edinburgh, EH8 9XP
//    Scotland
//    United Kingdom
//
// This program is free software;
// you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the
// GNU General Public License along with this program;
// if not, write to:
// Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301
// USA
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSDIndexRaster.cpp
// cpp file for the LSDIndexRaster object
// LSD stands for Land Surface Dynamics
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This object is written by
// Simon M. Mudd, University of Edinburgh
// David Milodowski, University of Edinburgh
// Martin D. Hurst, British Geological Survey
// <your name here>
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Version 0.0.1		20/08/2012
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//-----------------------------------------------------------------
//DOCUMENTATION URL: http://www.geos.ed.ac.uk/~s0675405/LSD_Docs/
//-----------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include "TNT/tnt.h"
#include "LSDIndexRaster.hpp"
using namespace std;
using namespace TNT;

#ifndef LSDIndexRaster_CPP
#define LSDIndexRaster_CPP

// operators
// SMM 2012
LSDIndexRaster& LSDIndexRaster::operator=(const LSDIndexRaster& rhs)
 {
  if (&rhs != this)
   {
    create(rhs.get_NRows(),rhs.get_NCols(),rhs.get_XMinimum(),rhs.get_YMinimum(),
           rhs.get_DataResolution(), rhs.get_NoDataValue(), rhs.get_RasterData(),
           rhs.get_GeoReferencingStrings() );
   }
  return *this;
 }

// the create function. This is default and throws an error
// SMM 2012
void LSDIndexRaster::create()
{
	//cout << "You need to initialize with a filename!" << endl;
	//exit(EXIT_FAILURE);
}

// this creates a raster using an infile
// SMM 2012
void LSDIndexRaster::create(string filename, string extension)
{
	read_raster(filename,extension);
}

// this creates a raster filled with the data in data
// SMM 2012
void LSDIndexRaster::create(int nrows, int ncols, float xmin, float ymin,
            float cellsize, int ndv, Array2D<int> data)
{
	NRows = nrows;
	NCols = ncols;
	XMinimum = xmin;
	YMinimum = ymin;
	DataResolution = cellsize;
	NoDataValue = ndv;

	RasterData = data.copy();

	if (RasterData.dim1() != NRows)
	{
		cout << "dimesntion of data is not the same as stated in NRows!" << endl;
		exit(EXIT_FAILURE);
	}
	if (RasterData.dim2() != NCols)
	{
		cout << "dimesntion of data is not the same as stated in NRows!" << endl;
		exit(EXIT_FAILURE);
	}

}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// similar to above but contains the georeferencing
// SMM 2014
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void LSDIndexRaster::create(int nrows, int ncols, float xmin, float ymin,
            float cellsize, int ndv, Array2D<int> data, map<string,string> GRS_map)
{
	NRows = nrows;
	NCols = ncols;
	XMinimum = xmin;
	YMinimum = ymin;
	DataResolution = cellsize;
	NoDataValue = ndv;
	GeoReferencingStrings = GRS_map;

	RasterData = data.copy();

	if (RasterData.dim1() != NRows)
	{
		cout << "dimesntion of data is not the same as stated in NRows!" << endl;
		exit(EXIT_FAILURE);
	}
	if (RasterData.dim2() != NCols)
	{
		cout << "dimesntion of data is not the same as stated in NRows!" << endl;
		exit(EXIT_FAILURE);
	}

}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// this function reads a DEM
// One has to provide both the filename and the extension
// the '.' between the filename and extension is not included
// for example, if the full filename is test.asc
// then
// filename = "test"
// and
// ext = "asc"
// The full filename coult also be "test.01.asc"
// so filename would be "test.01"
// and ext would again be "asc"
// SMM 2012
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void LSDIndexRaster::read_raster(string filename, string extension)
{
	string string_filename;
	string dot = ".";
	string_filename = filename+dot+extension;
	cout << "The filename is " << string_filename << endl;


	if (extension == "asc")
	{
		// open the data file
		ifstream data_in(string_filename.c_str());

		//Read in raster data
		string str;			// a temporary string for discarding text

		// read the georeferencing data and metadata
		data_in >> str >> NCols >> str >> NRows
			    >> str >> XMinimum >> str >> YMinimum
		   		>> str >> DataResolution
			    >> str >> NoDataValue;

		cout << "Loading asc file; NCols: " << NCols << " NRows: " << NRows << endl
		     << "X minimum: " << XMinimum << " YMinimum: " << YMinimum << endl
		     << "Data Resolution: " << DataResolution << " and No Data Value: "
		     << NoDataValue << endl;

		// this is the array into which data is fed
		Array2D<int> data(NRows,NCols,NoDataValue);

		// read the data
		for (int i=0; i<NRows; ++i)
		{
			for (int j=0; j<NCols; ++j)
			{
				data_in >> data[i][j];
			}
		}
		data_in.close();

		// now update the objects raster data
		RasterData = data.copy();
	}
	else if (extension == "flt")
	{
		// float data (a binary format created by ArcMap) has a header file
		// this file must be opened first
		string header_filename;
		string header_extension = "hdr";
		header_filename = filename+dot+header_extension;

		ifstream ifs(header_filename.c_str());
		if( ifs.fail() )
		{
			cout << "\nFATAL ERROR: the header file \"" << header_filename
				 << "\" doesn't exist" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			string str;
			ifs >> str >> NCols >> str >> NRows
				>> str >> XMinimum >> str >> YMinimum
				>> str >> DataResolution
				>> str >> NoDataValue;
		}
		ifs.close();

		cout << "Loading asc file; NCols: " << NCols << " NRows: " << NRows << endl
			 << "X minimum: " << XMinimum << " YMinimum: " << YMinimum << endl
		     << "Data Resolution: " << DataResolution << " and No Data Value: "
		     << NoDataValue << endl;

		// this is the array into which data is fed
		Array2D<int> data(NRows,NCols,NoDataValue);

		// now read the DEM, using the binary stream option
		ifstream ifs_data(string_filename.c_str(), ios::in | ios::binary);
		if( ifs_data.fail() )
		{
			cout << "\nFATAL ERROR: the data file \"" << string_filename
			     << "\" doesn't exist" << endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			float temp;
			for (int i=0; i<NRows; ++i)
			{
				for (int j=0; j<NCols; ++j)
				{
					ifs_data.read(reinterpret_cast<char*>(&temp), sizeof(temp));
					data[i][j] = int(temp);
				}
			}
		}
		ifs_data.close();

		// now update the objects raster data
		RasterData = data.copy();
	}
	else if (extension == "bil")
	{
		// float data (a binary format created by ArcMap) has a header file
		// this file must be opened first
		string header_filename;
		string header_extension = "hdr";
		header_filename = filename+dot+header_extension;
		int NoDataExists = 0;

		ifstream ifs(header_filename.c_str());
		if( ifs.fail() )
		{
			cout << "\nFATAL ERROR: the header file \"" << header_filename
				 << "\" doesn't exist" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
		{
				  
			string str;
			ifs >> str;
			if (str != "ENVI")
			{
        cout << "\nFATAL ERROR: this is not an ENVI header file!, first line is: " 
             << str << endl;
        exit(EXIT_FAILURE);       
      }
      else
      {
        // the the rest of the lines
        int NChars = 5000; // need a big buffer beacause of the projection string
        char thisline[NChars];  
        vector<string> lines;
        while( ifs.getline(thisline, NChars) )
        {
          lines.push_back(thisline);
        }
        //cout << "Number of lines is: " << lines.size() << endl;
        //for(int i = 0; i< int(lines.size()); i++)
        //{
        //  cout << "Line["<<i<<"]: " << lines[i] << endl;
        //}
        
        // now loop through and get the number of rows
        int counter = 0;
        int NLines = int(lines.size());
        int this_NRows = 0;
        size_t found; 
        string str_find = "lines";
        while (counter < NLines)
        {
          found = lines[counter].find(str_find); 
          if (found!=string::npos)
          {
            // get the data using a stringstream
            istringstream iss(lines[counter]);
            iss >> str >> str >> str;
            this_NRows = atoi(str.c_str());
            //cout << "NRows = " << this_NRows << endl;
            NRows = this_NRows;
            
            // advance to the end so you move on to the new loop            
            counter = lines.size();            
          }
          else
          {
            counter++;
          }
        }

        // get the number of columns
        counter = 0;
        int this_NCols = 0;
        str_find = "samples";
        while (counter < NLines)
        {
          found = lines[counter].find(str_find); 
          if (found!=string::npos)
          {
            // get the data using a stringstream
            istringstream iss(lines[counter]);
            iss >> str >> str >> str;
            this_NCols = atoi(str.c_str());
            //cout << "NCols = " << this_NCols << endl;
            NCols = this_NCols;
            
            // advance to the end so you move on to the new loop            
            counter = lines.size();    
          }
          else
          {
            counter++;
          }
        }        

        // get data ignore value
        counter = 0;
        float this_NoDataValue = 0;
        str_find = "data ignore value";
        while (counter < NLines)
        {
          found = lines[counter].find(str_find); 
          if (found!=string::npos)
          {
            // get the data using a stringstream
            istringstream iss(lines[counter]);
            iss >> str >> str >> str >> str >> str;
            this_NoDataValue = atoi(str.c_str());
            //cout << "NCols = " << this_NCols << endl;
            NoDataValue = this_NoDataValue;
            
            NoDataExists = 1;   // set this to true
            
            // advance to the end so you move on to the new loop            
            counter = lines.size();    
          }
          else
          {
            counter++;
          }
        }   

        // get the map info
        counter = 0;
        string this_map_info = "empty";
        str_find = "map info";
        while (counter < NLines)
        {
          found = lines[counter].find(str_find); 
          if (found!=string::npos)
          {
            //cout << "Found map info on line " << counter << '\n';  
	    
	          // now split the line 
	          size_t start_pos;
	          size_t end_pos;
	          string open_curly_bracket = "{";
	          string closed_curly_bracket = "}";
	          start_pos = lines[counter].find(open_curly_bracket);
	          end_pos = lines[counter].find(closed_curly_bracket);
	          //cout << "startpos: " << start_pos << " and end pos: " << end_pos << endl;
	          string info_str = lines[counter].substr(start_pos+1, end_pos-start_pos-1);
	          //cout << "\nThe map info string is:\n" << info_str << endl;
	          string mi_key = "ENVI_map_info";
	          GeoReferencingStrings[mi_key] = info_str;

	          // now parse the string
	          vector<string> mapinfo_strings;
	          istringstream iss(info_str);
	          while( iss.good() )
	          {
	            string substr;
	            getline( iss, substr, ',' );
	            mapinfo_strings.push_back( substr );
	          }
	          XMinimum = atof(mapinfo_strings[3].c_str());	          	          
	          float YMax = atof(mapinfo_strings[4].c_str());
	          	       	   	          
	          DataResolution = atof(mapinfo_strings[5].c_str());
            
            // get Y minium
            YMinimum = YMax - NRows*DataResolution;	          

	          if (atof(mapinfo_strings[6].c_str()) != DataResolution)
	          {
	            cout << "Warning! Loading ENVI DEM, but X and Y data spacing are different!" << endl;
	          }

	          //cout << "Xmin: " << XMinimum << " YMin: " << YMinimum << " spacing: " 
            //     << DataResolution << endl;

            counter = lines.size();
          }
          else
          {
            counter++;
          }
        }     

        // get the projection string
        counter = 0;
        string this_coordinate_system_string = "empty";
        str_find = "coordinate system string";
        while (counter < NLines)
        {
          found = lines[counter].find(str_find); 
          if (found!=string::npos)
          {
            //cout << "Found coordinate system string on line " << counter << '\n';  

	          // now split the line 
	          size_t start_pos;
	          size_t end_pos;
	          string open_curly_bracket = "{";
	          string closed_curly_bracket = "}";
	          start_pos = lines[counter].find(open_curly_bracket);
	          end_pos = lines[counter].find(closed_curly_bracket);
	          //cout << "startpos: " << start_pos << " and end pos: " << end_pos << endl;
	          string csys_str = lines[counter].substr(start_pos+1, end_pos-start_pos-1);
	          //cout << "\nThe coordinate system string is:\n" << csys_str << endl;
	          string cs_key = "ENVI_coordinate_system";
	          GeoReferencingStrings[cs_key] = csys_str;
            counter = lines.size();
          }
          else
          {
            counter++;
          }
        }          
      }         
		}
		ifs.close(); 



     
		// this is the array into which data is fed
		if (NoDataExists == 0)
		{
      NoDataValue = -9999;
    }
		//bool set_NDV = false;
		Array2D<int> data(NRows,NCols,NoDataValue);

		// now read the DEM, using the binary stream option
		ifstream ifs_data(string_filename.c_str(), ios::in | ios::binary);
		if( ifs_data.fail() )
		{
			cout << "\nFATAL ERROR: the data file \"" << string_filename
			     << "\" doesn't exist" << endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			float temp;
			for (int i=0; i<NRows; ++i)
			{
				for (int j=0; j<NCols; ++j)
				{
					ifs_data.read(reinterpret_cast<char*>(&temp), sizeof(temp));
					
					data[i][j] = int(temp);
					if (data[i][j]<-1e10)
					{
            data[i][j] = NoDataValue;
          }					
				}
			}
		}
		ifs_data.close();

		cout << "Loading ENVI bil file; NCols: " << NCols << " NRows: " << NRows << endl
			 << "X minimum: " << XMinimum << " YMinimum: " << YMinimum << endl
		     << "Data Resolution: " << DataResolution << " and No Data Value: "
		     << NoDataValue << endl;

		// now update the objects raster data
		RasterData = data.copy();         
  }	
	else
	{
		cout << "You did not enter and approprate extension!" << endl
				  << "You entered: " << extension << " options are .flt and .asc" << endl;
		exit(EXIT_FAILURE);
	}


}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// write_raster
// this function writes a raster. One has to give the filename and extension
// currently the options are for .asc and .flt files
// SMM 2012
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void LSDIndexRaster::write_raster(string filename, string extension)
{
	string string_filename;
	string dot = ".";
	string_filename = filename+dot+extension;
	cout << "The filename is " << string_filename << endl;

	// this first bit of logic is for the asc file.
	if (extension == "asc")
	{
		// open the data file
		ofstream data_out(string_filename.c_str());

		if( data_out.fail() )
		{
			cout << "\nFATAL ERROR: unable to write to " << string_filename << endl;
			exit(EXIT_FAILURE);
		}

		data_out <<  "ncols         " << NCols
				<< "\nnrows         " << NRows
				<< "\nxllcorner     " << setprecision(14) << XMinimum
				<< "\nyllcorner     " << setprecision(14) << YMinimum
				<< "\ncellsize      " << DataResolution
				<< "\nNODATA_value  " << NoDataValue << endl;

		for (int i=0; i<NRows; ++i)
		{
			for (int j=0; j<NCols; ++j)
			{
				data_out << setprecision(6) << RasterData[i][j] << " ";
			}
			if (i != NRows-1) data_out << endl;
		}
		data_out.close();

	}
	else if (extension == "flt")
	{
		// float data (a binary format created by ArcMap) has a header file
		// this file must be opened first
		string header_filename;
		string header_extension = "hdr";
		header_filename = filename+dot+header_extension;

		ofstream header_ofs(header_filename.c_str());
		string str;
		header_ofs <<  "ncols         " << NCols
			<< "\nnrows         " << NRows
			<< "\nxllcorner     " << setprecision(14) << XMinimum
			<< "\nyllcorner     " << setprecision(14) << YMinimum
			<< "\ncellsize      " << DataResolution
			<< "\nNODATA_value  " << NoDataValue
			<< "\nbyteorder     LSBFIRST" << endl;
		header_ofs.close();

		// now do the main data
		ofstream data_ofs(string_filename.c_str(), ios::out | ios::binary);
		float temp;
		for (int i=0; i<NRows; ++i)
		{
			for (int j=0; j<NCols; ++j)
			{
				temp = float(RasterData[i][j]);
				data_ofs.write(reinterpret_cast<char *>(&temp),sizeof(temp));
			}
		}
		data_ofs.close();
	}
	else if (extension == "bil")
	{
		// float data (a binary format created by ArcMap) has a header file
		// this file must be opened first
		string header_filename;
		string header_extension = "hdr";
		header_filename = filename+dot+header_extension;

    // you need to strip the filename
    string frontslash = "/";
    size_t found = string_filename.find_last_of(frontslash); 
    //cout << "Found is: " << found << endl;
    
    int length = int(string_filename.length());
    string this_fname = string_filename.substr(found+1,length-found-1);
    //cout << "fname is: " << this_fname << endl;

    ofstream header_ofs(header_filename.c_str());
    string str;
    header_ofs <<  "ENVI" << endl;
    header_ofs << "description = {" << endl << this_fname << "}" << endl;
    header_ofs <<  "samples = " << NCols << endl;
    header_ofs <<  "lines = " << NRows << endl;
    header_ofs <<  "bands = 1" << endl;
    header_ofs <<  "header offset = 0" << endl;
    header_ofs <<  "file type = ENVI Standard" << endl;
    header_ofs <<  "data type = 4" << endl;
    header_ofs <<  "interleave = bsq" << endl;
    header_ofs <<  "byte order = 0" << endl;
    
    // now check to see if there are the map info and coordinate system 
    map<string,string>::iterator iter;
    string cs_str_key = "ENVI_coordinate_system";
    string mi_str_key = "ENVI_map_info";

    string cs_str;
    string mi_str;
    iter = GeoReferencingStrings.find(mi_str_key);
    if (iter != GeoReferencingStrings.end() )
    {
      mi_str = (*iter).second;
      //cout << "Map info system string exists, it is: " << mi_str << endl;
      header_ofs <<  "map info = {"<<mi_str<<"}" << endl;
    }
    else
    {
      cout << "Warning, writing ENVI file but no map info string" << endl;
    } 
    iter = GeoReferencingStrings.find(cs_str_key);
    if (iter != GeoReferencingStrings.end() )
    {
      cs_str = (*iter).second;
      //cout << "Coord, system string exists, it is: " << cs_str << endl;
      header_ofs <<  "coordinate system string = {"<<cs_str<<"}" << endl;
    }
    else
    {
      cout << "Warning, writing ENVI file but no coordinate system string" << endl;
    }
    header_ofs <<  "data ignore value = " << NoDataValue << endl;
 	      
    header_ofs.close();

    // now do the main data
    ofstream data_ofs(string_filename.c_str(), ios::out | ios::binary);
    float temp;
    for (int i=0; i<NRows; ++i)
    {
      for (int j=0; j<NCols; ++j)
      {
        temp = float(RasterData[i][j]);
	      data_ofs.write(reinterpret_cast<char *>(&temp),sizeof(temp));
      }
    }
    data_ofs.close();
  }	
	else
	{
		cout << "You did not enter and approprate extension!" << endl
				  << "You entered: " << extension << " options are .flt and .asc" << endl;
		exit(EXIT_FAILURE);
	}

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Calculate the minimum bounding rectangle for an LSDIndexRaster Object and crop out
// all the surrounding NoDataValues to reduce the size and load times of output
// rasters.
//
// Ideal for use with chi analysis tools which output basin and chi m value rasters
// which can be predominantly no data. As an example, a 253 Mb file can be reduced to
// ~5 Mb with no loss or resampling of data.
//
// Returns A trimmed LSDIndexRaster object.
//
// SWDG 22/08/13
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
LSDIndexRaster LSDIndexRaster::RasterTrimmer(){

  //minimum index value in a column
  int a = 0;
  int min_col = 100000; //a big number

  for (int row = 0; row < NRows; ++row){
    a = 0;
    while (RasterData[row][a] == NoDataValue && a < NCols-1){
      ++a;
    }
    if (min_col > a){
      min_col = a;
    }
  }

  //maximum index value in a column
  a = NCols - 1;
  int max_col = 0; //a small number

  for (int row = 0; row < NRows; ++row){
    a = NCols - 1;
    while (RasterData[row][a] == NoDataValue && a > 0){
      --a;
    }
    if (max_col < a){
      max_col = a;
    }
  }

  //minimum index value in a row
  a = 0;
  int min_row = 100000; //a big number

  for (int col = 0; col < NCols; ++col){
    a = 0;
    while (RasterData[a][col] == NoDataValue && a < NRows - 1){
      ++a;
    }
    if (min_row > a){
      min_row = a;
    }
  }

  //maximum index value in a row
  a = NRows - 1;
  int max_row = 0; //a small number

  for (int col = 0; col < NCols; ++col){
    a = NRows - 1;
    while (RasterData[a][col] == NoDataValue && a > 0){
      --a;
    }
    if (max_row < a){
      max_row = a;
    }
  }

  // create new row and col sizes taking account of zero indexing
  int new_row_dimension = (max_row-min_row) + 1;
  int new_col_dimension = (max_col-min_col) + 1;

  Array2D<int>TrimmedData(new_row_dimension, new_col_dimension, NoDataValue);

  //loop over min bounding rectangle and store it in new array of shape new_row_dimension x new_col_dimension
  int TrimmedRow = 0;
  int TrimmedCol = 0;
  for (int row = min_row - 1; row < max_row; ++row){
    for(int col = min_col - 1; col < max_col; ++col){
      TrimmedData[TrimmedRow][TrimmedCol] = RasterData[row][col];
      ++TrimmedCol;
    }
    ++TrimmedRow;
    TrimmedCol = 0;
  }

  //calculate lower left corner coordinates of new array
  float new_XLL = ((min_col - 1) * DataResolution) + XMinimum;
  float new_YLL = YMinimum + ((NRows - (max_row + 0)) * DataResolution);

  LSDIndexRaster TrimmedRaster(new_row_dimension, new_col_dimension, new_XLL,
                          new_YLL, DataResolution, NoDataValue, TrimmedData);

  return TrimmedRaster;

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Make LSDIndexRaster object using a 'template' raster and an Array2D of data.
// SWDG 2/9/13
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
LSDIndexRaster LSDIndexRaster::LSDRasterTemplate(Array2D<int> InputData){

  //do a dimensions check and exit on failure
  if (InputData.dim1() == NRows && InputData.dim2() == NCols){
    LSDIndexRaster OutputRaster(NRows, NCols, XMinimum, YMinimum, DataResolution, NoDataValue, InputData,GeoReferencingStrings);
    return OutputRaster;
  }
  else{
   	cout << "Array dimensions do not match template LSDIndexRaster object" << endl;
		exit(EXIT_FAILURE);
  }

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// This function implements the thinning algorithm described in Rosenfeld and
// Kak (1982).  It takes a binary map and turns it into a continuous single 
// thread skeleton.  At present, pixels at the limits of the raster are
// automatically set to 0.  If it is necessary to extend the skeleton to the 
// edge, this should be a straightforward operation afterwards.
//
// Added by DTM 28/10/2013
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
LSDIndexRaster LSDIndexRaster::thin_to_single_thread_network()
{
  Array2D<int> Skeleton(NRows,NCols,NoDataValue);
  Array2D<int> SkeletonUpdate(NRows,NCols,NoDataValue);
  // Step 1:- outlne borders
  // Corners of array
  // SE
  if(RasterData[0][0]!=NoDataValue)
  { 
    if(RasterData[0][0]==0) Skeleton[0][0]=0;
    else if((RasterData[1][0]==0) && (RasterData[0][1]==0)) Skeleton[0][0]=1;
    else if((RasterData[1][0]==0) || (RasterData[0][1]==0)) Skeleton[0][0]=2;
    else Skeleton[0][0]=3;
  }
  // SW
  if(RasterData[0][NCols-1]!=NoDataValue)
  { 
    if(RasterData[0][NCols-1]==0) Skeleton[0][NCols-1]=0;
    else if((RasterData[1][NCols-1]==0) && (RasterData[0][NCols-2]==0)) Skeleton[0][NCols-1]=1;
    else if((RasterData[1][NCols-1]==0) || (RasterData[0][NCols-2]==0)) Skeleton[0][NCols-1]=2;
    else Skeleton[0][0]=3;
  }
  // NE
  if(RasterData[NRows-1][0]!=NoDataValue)
  { 
    if(RasterData[NRows-1][0]==0) Skeleton[NRows-1][0]=0;
    else if((RasterData[NRows-2][0]==0) && (RasterData[NRows-1][1]==0)) Skeleton[NRows-1][0]=1;
    else if((RasterData[NRows-2][0]==0) || (RasterData[NRows-1][1]==0)) Skeleton[NRows-1][0]=2;
    else Skeleton[NRows-1][0]=3;
  }
  // SE
  if(RasterData[NRows-1][NCols-1]!=NoDataValue)
  { 
    if(RasterData[NRows-1][NCols-1]==0) Skeleton[NRows-1][NCols-1]=0;
    else if((RasterData[NRows-2][NCols-1]==0) && (RasterData[NRows-1][NCols-2]==0)) Skeleton[NRows-1][NCols-1]=1;
    else if((RasterData[NRows-2][NCols-1]==0) || (RasterData[NRows-1][NCols-2]==0)) Skeleton[NRows-1][NCols-1]=2;
    else Skeleton[NRows-1][NCols-1]=3;
  }
  // Edges of Array
  for (int i = 1; i < NRows-1; ++i)
  {
    //North
    if(RasterData[i][0]!=NoDataValue)
    {
      if(RasterData[i][0]==0) Skeleton[i][0]=0;
      else if((RasterData[i+1][0]==0) && (RasterData[i-1][0]==0)) Skeleton[i][0]=1;
      else if((RasterData[i+1][0]==0) || (RasterData[i-1][0]==0) || (RasterData[i][1]==0)) Skeleton[i][0]=2;
      else Skeleton[i][0]=3;
    }
    // South
    if(RasterData[i][NCols-1]!=NoDataValue)
    { 
      if(RasterData[i][NCols-1]==0) Skeleton[i][NCols-1]=0;
      else if((RasterData[i+1][NCols-1]==0) && (RasterData[i-1][NCols-1]==0)) Skeleton[i][NCols-1]=1;
      else if((RasterData[i+1][NCols-1]==0) || (RasterData[i-1][NCols-1]==0) || (RasterData[i][NCols-2]==0)) Skeleton[i][NCols-1]=2;
      else Skeleton[i][NCols-1]=3;
    }
  }
  for (int j = 1; j < NRows-1; ++j)
  {  
    // East
    if(RasterData[0][j]!=NoDataValue)
    {
      if(RasterData[0][j]==0) Skeleton[0][j]=0;
      else if((RasterData[0][j+1]==0) && (RasterData[0][j-1]==0)) Skeleton[0][j]=1;
      else if((RasterData[0][j+1]==0) || (RasterData[0][j-1]==0) || (RasterData[1][j]==0)) Skeleton[0][j]=2;
      else Skeleton[0][j]=3;
    }  
    // West
    if(RasterData[NRows-1][j]!=NoDataValue)
    {
      if(RasterData[NRows-1][j]==0) Skeleton[NRows-1][j]=0;
      else if((RasterData[NRows-1][j+1]==0) && (RasterData[NRows-1][j-1]==0)) Skeleton[NRows-1][j]=1;
      else if((RasterData[NRows-1][j+1]==0) || (RasterData[NRows-1][j-1]==0) || (RasterData[NRows-2][j]==0)) Skeleton[NRows-1][j]=2;
      else Skeleton[NRows-1][j]=3;
    }
  }
  // for rest of array
  for (int i = 1; i < NRows-1; ++i)
  {
    for (int j = 1; j< NCols-1; ++j)
    {
      if(RasterData[i][j] == 1)
      {
        // Skeleton
        if ( ((RasterData[i+1][j] == 0) && (RasterData[i-1][j] == 0)) || (RasterData[i][j+1] == 0 && (RasterData[i][j-1] == 0)) ) Skeleton[i][j] = 1;
        // Non skeleton boundaries
        else if(RasterData[i-1][j] == 0 || RasterData[i+1][j] == 0 || RasterData[i][j-1] == 0 || RasterData[i][j-1] == 0) Skeleton = 2;
        // Non boundary
        else Skeleton = 3;
      }
      else Skeleton = 0;
    }
  }
  //----------------------------------------------------------------------------
  // Step 2:- Now loop through the array again, progressively searching for 
  // N,E,S,W boundaries
  bool IsSkeleton = false;
  SkeletonUpdate = Skeleton.copy();
  while (IsSkeleton == false)
  {
    IsSkeleton = true;
    // NORTH BOUNDARIES
    // For the edges - E 
    for (int i = 1; i < NRows-1; ++i) // Northern most row can't have a Northern boundary and it makes little sense to do the Southern most row as there is no row beneath!
    { 
      int j=0;
      if((Skeleton[i][j] == 2 && Skeleton[i-1][j] == 0))
      {
        IsSkeleton = false;
//         // Logic for case where you are at an end, but the feature is two 
//         // pixels wide - do not want to truncate skeleton here!
//         if((Skeleton[i][j+1]==0) && (Skeleton[i+1][j+1]==0) && (Skeleton[i+1][j]==2)) SkeletonUpdate[i][j] = 2;
//         else
//         {
          SkeletonUpdate[i][j] = 0;
          // Check to see whether pixel to South is a border - if yes, make it
          // a skeleton pixel
          if (Skeleton[i+1][j] == 2) SkeletonUpdate[i+1][j] = 1;
          // Otherwise it will be the boundary pixel for the next round
          else SkeletonUpdate[i+1][j] = 2;
//         }
      }
      // W
      j=NCols-1;
      if((Skeleton[i][j] == 2 && Skeleton[i-1][j] == 0))
      {
        IsSkeleton = false;
//         if((Skeleton[i][j-1]==0) && (Skeleton[i+1][j-1]==0) && (Skeleton[i+1][j]==2)) SkeletonUpdate[i][j] = 2; 
//         else
//         {
          SkeletonUpdate[i][j] = 0;
          if (Skeleton[i+1][j] == 2) SkeletonUpdate[i+1][j] = 1;
          else SkeletonUpdate[i+1][j] = 2;
//         }
      }      
    }

    // for the rest of the array    
    for (int i = 1; i < NRows-1; ++i)
    {
      for (int j = 1; j< NCols-1; ++j)
      {            
        if((Skeleton[i][j] == 2) && (Skeleton[i-1][j] == 0))
        {
          IsSkeleton = false;
          // Logic for case where you are at an end, but the feature is two 
          // pixels wide - do not want to truncate skeleton here!
          if( ((Skeleton[i][j+1]==0) && (Skeleton[i+1][j+1]==0) && (Skeleton[i+1][j]==2))
           || ((Skeleton[i][j-1]==0) && (Skeleton[i+1][j-1]==0) && (Skeleton[i+1][j]==2)) )
          {
            SkeletonUpdate[i][j] = 2;
          }  
          else
          {
            SkeletonUpdate[i][j] = 0;
            // Check to see whether pixel to South is a border - if yes, make it
            // a skeleton pixel
            if (Skeleton[i+1][j] == 2) SkeletonUpdate[i+1][j] = 1;
            // Otherwise it will be the boundary pixel for the next round
            else SkeletonUpdate[i+1][j] = 2;
          }
        }
      }
    }
    Skeleton = SkeletonUpdate.copy();
    
    // SOUTH BOUNDARIES
    // For the edges - E and W
    for (int i = 1; i < NRows - 1; ++i)
    { 
      int j=0;
      if((Skeleton[i][j] == 2) && (Skeleton[i+1][j] == 0))
      {
        IsSkeleton = false;
        SkeletonUpdate[i][j] = 0;
        // Check to see whether pixel to South is a border - if yes, make it
        // a skeleton pixel
        if (Skeleton[i-1][j] == 2) SkeletonUpdate[i-1][j] = 1;
        // Otherwise it will be the boundary pixel for the next round
        else SkeletonUpdate[i-1][j] = 2;
      }
      j=NCols-1;
      if((Skeleton[i][j] == 2 && Skeleton[i+1][j] == 0))
      {
        IsSkeleton = false;
        SkeletonUpdate[i][j] = 0;
        if (Skeleton[i-1][j] == 2) SkeletonUpdate[i-1][j] = 1;
        else SkeletonUpdate[i-1][j] = 2;
      }      
    }

    // for the rest of the array    
    for (int i = 1; i < NRows-1; ++i)
    {
      for (int j = 1; j< NCols-1; ++j)
      {            
        if((Skeleton[i][j] == 2 && Skeleton[i+1][j] == 0))
        {
          IsSkeleton = false;
          // Logic for case where you are at an end, but the feature is two 
          // pixels wide - do not want to truncate skeleton here!
          if( ((Skeleton[i][j+1]==0) && (Skeleton[i-1][j+1]==0) && (Skeleton[i-1][j]==2))
           || ((Skeleton[i][j-1]==0) && (Skeleton[i-1][j-1]==0) && (Skeleton[i-1][j]==2)) )
          {
            SkeletonUpdate[i][j] = 2;
          }  
          else
          {
            SkeletonUpdate[i][j] = 0;
            // Check to see whether pixel to North is a border - if yes, make it
            // a skeleton pixel
            if (Skeleton[i-1][j] == 2) SkeletonUpdate[i-1][j] = 1;
            // Otherwise it will be the boundary pixel for the next round
            else SkeletonUpdate[i-1][j] = 2;
          }
        }
      }
    }
    Skeleton = SkeletonUpdate.copy();
    // EAST-FACING BOUNDARIES
    // For the edges - N and S
    for (int j = 1; j < NCols; ++j)
    { 
      // South edge
      int i=0;
      if((Skeleton[i][j] == 2 && Skeleton[i][j-1] == 0))
      {
        IsSkeleton = false;
        // Logic for case where you are at an end, but the feature is two 
        // pixels wide - do not want to truncate skeleton here!
        SkeletonUpdate[i][j] = 0;
        // Check to see whether pixel to West is a border - if yes, make it
        // a skeleton pixel
        if (Skeleton[i][j+1] == 2) SkeletonUpdate[i][j+1] = 1;
        // Otherwise it will be the boundary pixel for the next round
        else SkeletonUpdate[i][j+1] = 2;
      }
      // North edge
      i=NRows-1;
      if((Skeleton[i][j] == 2 && Skeleton[i][j-1] == 0))
      {
        IsSkeleton = false;
        SkeletonUpdate[i][j] = 0;
        if (Skeleton[i][j+1] == 2) SkeletonUpdate[i][j+1] = 1;
        else SkeletonUpdate[i][j+1] = 2;
      }      
    }
    
    // for the rest of the array    
    for (int i = 1; i < NRows-1; ++i)
    {
      for (int j = 1; j< NCols-1; ++j)
      {
        // For rest of the array              
        if((Skeleton[i][j] == 2 && Skeleton[i][j-1] == 0))
        {
          IsSkeleton = false;
          // Logic for case where you are at an end, but the feature is two 
          // pixels wide - do not want to truncate skeleton here!
          if( ((Skeleton[i+1][j]==0) && (Skeleton[i+1][j+1]==0) && (Skeleton[i][j+1]==2))
           || ((Skeleton[i-1][j]==0) && (Skeleton[i-1][j+1]==0) && (Skeleton[i][j+1]==2)) )
          {
            SkeletonUpdate[i][j] = 2;
          }  
          else
          {
            SkeletonUpdate[i][j] = 0;
            // Check to see whether pixel to West is a border - if yes, make it
            // a skeleton pixel
            if (Skeleton[i][j+1] == 2) SkeletonUpdate[i][j+1] = 1;
            // Otherwise it will be the boundary pixel for the next round
            else SkeletonUpdate[i][j+1] = 2;
          }
        }
      }
    }
    Skeleton = SkeletonUpdate.copy();
    // WEST-FACING BOUNDARIES
    // For the edges - N and S
    for (int j = 1; j < NCols; ++j)
    { 
      // South edge
      int i=0;
      if((Skeleton[i][j] == 2 && Skeleton[i][j+1] == 0))
      {
        IsSkeleton = false;
        // Logic for case where you are at an end, but the feature is two 
        // pixels wide - do not want to truncate skeleton here!
        SkeletonUpdate[i][j] = 0;
        // Check to see whether pixel to West is a border - if yes, make it
        // a skeleton pixel
        if (Skeleton[i][j-1] == 2) SkeletonUpdate[i][j-1] = 1;
        // Otherwise it will be the boundary pixel for the next round
        else SkeletonUpdate[i][j-1] = 2;
      }
      // North edge
      i=NRows-1;
      if((Skeleton[i][j] == 2 && Skeleton[i][j+1] == 0))
      {
        IsSkeleton = false;
        SkeletonUpdate[i][j] = 0;
        if (Skeleton[i][j-1] == 2) SkeletonUpdate[i][j-1] = 1;
        else SkeletonUpdate[i][j-1] = 2;
      }      
    }
    // For rest of the array 
    for (int i=1; i<NRows-1; ++i)
    {
      for (int j=1; j<NCols-1; ++j)
      {                     
        if((Skeleton[i][j] == 2 && Skeleton[i][j+1] == 0))
        {
          IsSkeleton = false;
          // Logic for case where you are at an end, but the feature is two 
          // pixels wide - do not want to truncate skeleton here!
          if( ((Skeleton[i+1][j]==0) && (Skeleton[i+1][j-1]==0) && (Skeleton[i][j-1]==2))
           || ((Skeleton[i-1][j]==0) && (Skeleton[i-1][j-1]==0) && (Skeleton[i][j-1]==2)) )
          {
            SkeletonUpdate[i][j] = 2;
          }  
          else
          {
            SkeletonUpdate[i][j] = 0;
            // Check to see whether pixel to West is a border - if yes, make it
            // a skeleton pixel
            if (Skeleton[i][j-1]==2) SkeletonUpdate[i][j-1] = 1;
            // Otherwise it will be the boundary pixel for the next round
            else SkeletonUpdate[i][j-1] = 2;
          }
        }
      }
    }
    Skeleton = SkeletonUpdate.copy();    
  }
  
  // finally, loop through, and remove any remaining 3-pixels, which should only
  // be skeleton pixels that are in this arrangement:
  //                            0 1 0
  //                            1 3 1
  //                            0 1 0
  for (int i = 0; i < NRows; ++i)
  {
    for (int j = 0; j< NCols; ++j)
    {
      if (Skeleton[i][j] == 3) Skeleton[i][j] = 1;
    }
  }
  
  LSDIndexRaster skeleton_raster(NRows,NCols,XMinimum,YMinimum,DataResolution,NoDataValue,Skeleton,GeoReferencingStrings);
	return skeleton_raster;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Method to resample an LSDIndexRaster to a lower resolution. 
// OutputResolution is the resolution in spatial units to be resampled to.
// Returns an LSDRaster resampled to the OutputResolution.
// SWDG 17/3/14
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=   
LSDIndexRaster LSDIndexRaster::Resample(float OutputResolution){

  if (OutputResolution < DataResolution){
    cout << "Your resample resolution of " << OutputResolution << " is lower that the current data resolution " << DataResolution << endl;
	  exit(EXIT_FAILURE);
  } 

  int NewNRows = (NRows*DataResolution/OutputResolution); 
  int NewNCols = (NCols*DataResolution/OutputResolution);

  Array2D<int> Resampled(NewNRows, NewNCols, NoDataValue);
  
  int centre_i;
  int centre_j;   
  
  float ResolutionRatio = OutputResolution/DataResolution;
  
  for (int i = 0; i < NewNRows; ++i){
    for (int j = 0; j < NewNCols; ++j){
    
      //find the centre of the new grid in the old grid units
      centre_i = (i*ResolutionRatio) + (ResolutionRatio/2);
      centre_j = (j*ResolutionRatio) + (ResolutionRatio/2);
     
      Resampled[i][j] = RasterData[centre_i][centre_j];           
      
    }
  }                              

  LSDIndexRaster OutputRaster(NewNRows,NewNCols,XMinimum,YMinimum,OutputResolution,NoDataValue,Resampled);
  return OutputRaster;

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  
// Method to combine two rasters, ignoring nodata.  
// SWDG 17/6/14
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  
LSDIndexRaster LSDIndexRaster::CombineBinaryNetwork(LSDIndexRaster& Network1, LSDIndexRaster& Network2){

  Array2D<int> CombinedNetworkArray(NRows, NCols, NoDataValue);

   for (int i = 0; i < NRows; ++i){
    for (int j = 0; j < NCols; ++j){
   
      if (Network1.get_data_element(i,j) == NoDataValue && Network2.get_data_element(i,j) == NoDataValue){
        CombinedNetworkArray[i][j] = NoDataValue;
      }
      else if ((Network1.get_data_element(i,j) == NoDataValue && Network2.get_data_element(i,j) != NoDataValue)){
        CombinedNetworkArray[i][j] = Network2.get_data_element(i,j);  
      }
      else if ((Network1.get_data_element(i,j) != NoDataValue && Network2.get_data_element(i,j) == NoDataValue)){
        CombinedNetworkArray[i][j] = Network1.get_data_element(i,j);  
      }
      else if ((Network1.get_data_element(i,j) != NoDataValue && Network2.get_data_element(i,j) != NoDataValue)){
        CombinedNetworkArray[i][j] = Network2.get_data_element(i,j) + Network1.get_data_element(i,j);  
      }
      
    }
  }

  LSDIndexRaster CombinedNetwork(NRows, NCols, XMinimum, YMinimum, DataResolution, NoDataValue, CombinedNetworkArray,GeoReferencingStrings);
  return CombinedNetwork;

}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#endif
