/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataWriter.h>

#include <itkDataSHImageWriterPluginExport.h>
#include <itkImageIOBase.h>

class ITKDATASHIMAGEWRITERPLUGIN_EXPORT itkDataSHImageWriterBase : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    itkDataSHImageWriterBase();
    virtual ~itkDataSHImageWriterBase();

    virtual QStringList handled() const;

    virtual QStringList supportedFileExtensions() const;

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

private:
    template <class PixelType>
    bool write(const QString& path, PixelType dummyArgument);

protected:
    static QStringList s_handled();

    itk::ImageIOBase::Pointer io;
};
