#include <GUI/Qt/GLWidget/GLWidget.hpp>


void GLWidget::AnimationInitializer ( )
{
        // Animation Controls
        fps = 0;
        videoSequence = 0;
        frames.resize ( 4000 );

        // Quaternion Slerp
        sourcePosition_ = Eigen::Quaternionf ( 0.0 , 1.0 , 0.0 , 0.0 );
        sourcePosition_.normalize ( );

        targetPosition_ = Eigen::Quaternionf ( 0.0 , 0.0 , 0.0 , 1.0 );
        targetPosition_.normalized ( );

        number_of_takes_ = 0;
        take_index_ = 0;

        takes_.resize ( 10 , Animation ( ) );

        time_steps_ = 0.0;

        play_ = false;

        delta_time.start();

        updateTimer_.setSingleShot ( false );
        connect ( &updateTimer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( gameLooping ( ) ) );
        updateTimer_.start(0);

        fpsTimer_.setSingleShot ( false );
        connect ( &fpsTimer_ , SIGNAL ( timeout ( ) ) , this , SLOT ( fpsCounter( ) ) );
        fpsTimer_.start ( 1000 );
}

void GLWidget::flush()
{

//        for (std::size_t index = 0; index < videoSequence; index++)
//        {
//                frames[index].save("frame"+QString::number(index)+".tif","TIFF",100);
//        }
//
//        QProcess _process;
//        _process.start(QString("echo"), QStringList("-la"));
//        _process.waitForFinished();

        ofstream Morison_File ( "RenderingTimes.csv" );         //Opening file to print info to
        //Morison_File << "Genertation;Rendering;SSAO;MeanFilter" << endl;          //Headings for file
        float F;
        for ( std::size_t t = 0; t < renderPass_; t++ )
        {

                Morison_File << cutawayGenerationTimes_[t] << ";"
                             << renderingCutawayTimes_[t] << ";"
                             << renderingPrimaryTimes_[t] << ";"
                             << SSAOBlurCutawayTimes_[t] << ";"
                             << MeanFilterTimes_[t] <<  ";" << "\n";

        }

        Morison_File.close ( );

        renderPass_ = 0;

        cutawayGenerationTimes_.clear();
        renderingCutawayTimes_.clear();
        SSAOBlurCutawayTimes_.clear();
        MeanFilterTimes_.clear();


}
// From libQGLViewer
//---------------------------------------------------------------------
/*! Returns the slerp interpolation of Quaternions \p a and \p b, at time \p t.

 \p t should range in [0,1]. Result is \p a when \p t=0 and \p b when \p t=1.

 When \p allowFlip is \c true (default) the slerp interpolation will always use the "shortest path"
 between the Quaternions' orientations, by "flipping" the source Quaternion if needed (see
 negate()). */
Eigen::Quaternionf GLWidget::slerp ( const Eigen::Quaternionf& a , const Eigen::Quaternionf& b , float t , bool allowFlip )
{
        double cosAngle = a.dot ( b );

        double c1, c2;
        // Linear interpolation for close orientations
        if ( ( 1.0 - fabs ( cosAngle ) ) < 0.01 )
        {
                c1 = 1.0 - t;
                c2 = t;
        }
        else
        {
                // Spherical interpolation
                double angle = acos ( fabs ( cosAngle ) );
                double sinAngle = sin ( angle );
                c1 = sin ( angle * ( 1.0 - t ) ) / sinAngle;
                c2 = sin ( angle * t ) / sinAngle;
        }

        // Use the shortest path
        if ( allowFlip && ( cosAngle < 0.0 ) )
                c1 = -c1;

        return Eigen::Quaternionf ( c1 * a.w ( ) + c2 * b.w ( ) , c1 * a.x ( ) + c2 * b.x ( ) , c1 * a.y ( ) + c2 * b.y ( ) , c1 * a.z ( ) + c2 * b.z ( ) );
}

//Useful for smooth spline interpolation of Quaternion with squad() and slerp(). */
Eigen::Quaternionf GLWidget::squadTangent ( const Eigen::Quaternionf& before , const Eigen::Quaternionf& center , const Eigen::Quaternionf& after )
{
        Eigen::Quaternionf l1 = lnDif ( center , before );
        Eigen::Quaternionf l2 = lnDif ( center , after );
        Eigen::Quaternionf e;

        e = Eigen::Quaternionf ( -0.25 * ( l1.w ( ) + l2.w ( ) ) , -0.25 * ( l1.x ( ) + l2.x ( ) ) , -0.25 * ( l1.y ( ) + l2.y ( ) ) , -0.25 * ( l1.z ( ) + l2.z ( ) ) );

        e = center * ( exp ( e ) );

        // if (Quaternion::dot(e,b) < 0.0)
        // e.negate();

        return e;
}

/*! Returns the slerp interpolation of the two Quaternions \p a and \p b, at time \p t, using
 tangents \p tgA and \p tgB.

 The resulting Quaternion is "between" \p a and \p b (result is \p a when \p t=0 and \p b for \p
 t=1).

 Use squadTangent() to define the Quaternion tangents \p tgA and \p tgB. */
Eigen::Quaternionf GLWidget::squad ( const Eigen::Quaternionf& a , const Eigen::Quaternionf& tgA , const Eigen::Quaternionf& tgB , const Eigen::Quaternionf& b , float t )
{
        Eigen::Quaternionf ab = slerp ( a , b , t , false );
        Eigen::Quaternionf tg = slerp ( tgA , tgB , t , false );
        return slerp ( ab , tg , 2.0 * t * ( 1.0 - t ) , false );
}

/*! Returns the logarithm of the Quaternion. See also exp(). */
Eigen::Quaternionf GLWidget::log ( const Eigen::Quaternionf& q ) const
{
        double len = sqrt ( q.x ( ) * q.x ( ) + q.y ( ) * q.y ( ) + q.z ( ) * q.z ( ) );

        if ( len < 1E-6 )
                return Eigen::Quaternionf ( 0.0 , q.x ( ) , q.y ( ) , q.z ( ) );
        else
        {
                double coef = acos ( q.w ( ) ) / len;
                return Eigen::Quaternionf ( 0.0 , q.x ( ) * coef , q.y ( ) * coef , q.z ( ) * coef );
        }
}

/*! Returns the exponential of the Eigen::Quaternionf. See also log(). */
Eigen::Quaternionf GLWidget::exp ( const Eigen::Quaternionf& q ) const
{
        double theta = sqrt ( q.x ( ) * q.x ( ) + q.y ( ) * q.y ( ) + q.z ( ) * q.z ( ) );

        if ( theta < 1E-6 )
                return Eigen::Quaternionf ( std::cos ( theta ) , q.x ( ) , q.y ( ) , q.z ( ) );
        else
        {
                double coef = std::sin ( theta ) / theta;
                return Eigen::Quaternionf ( std::cos ( theta ) , q.x ( ) * coef , q.y ( ) * coef , q.z ( ) * coef );
        }
}

/*! Returns log(a. inverse() * b). Useful for squadTangent(). */
Eigen::Quaternionf GLWidget::lnDif ( const Eigen::Quaternionf& a , const Eigen::Quaternionf& b )
{
        Eigen::Quaternionf dif = Eigen::Quaternionf ( a.w ( ) , -a.x ( ) , -a.y ( ) , -a.z ( ) ) * b;
        dif.normalize ( );
        return log ( dif );
}

void GLWidget::calculeTangents ( int index )
{

        Eigen::Quaternionf prevQ = takes_[index].keyframes_[0];
        Eigen::Quaternionf kf;
        for ( std::size_t i = 0; i < takes_[index].number_of_keyframes_ - 1; ++i )
        {

                if ( prevQ.dot ( takes_[index].keyframes_[i] ) < 0.0 )
                {
                        takes_[index].keyframes_[i] = Eigen::Quaternionf ( -kf.w ( ) , -kf.x ( ) , -kf.y ( ) , -kf.z ( ) );
                }
                prevQ = takes_[index].keyframes_[i];

                takes_[index].keyframes_[i].normalize ( );
        }

        Eigen::Quaternionf prev = takes_[index].keyframes_[0];
        kf = takes_[index].keyframes_[0];

        for ( std::size_t i = 1; i < takes_[index].number_of_keyframes_ - 2; ++i )
        {
                Eigen::Quaternionf next = takes_[index].keyframes_[i];
                takes_[index].tangents_[i - 1] = squadTangent ( prev , kf , next );
                prev = kf;
                kf = next;
                std::cout << " calRecTagent " << takes_[index].tangents_[i - 1].coeffs ( ) << std::endl;

                takes_[index].tangents_[i - 1].normalize ( );
        }

        takes_[index].tangents_[takes_[index].number_of_keyframes_ - 1] = squadTangent ( takes_[index].keyframes_[takes_[index].number_of_keyframes_ - 1] , takes_[index].keyframes_[takes_[index].number_of_keyframes_ - 1] ,
                                                                                                                                        takes_[index].keyframes_[takes_[index].number_of_keyframes_ - 1] );
        takes_[index].tangents_[0] = squadTangent ( takes_[index].keyframes_[0] , takes_[index].keyframes_[0] , takes_[index].keyframes_[0] );

        takes_[index].tangents_[takes_[index].number_of_keyframes_ - 1].normalize ( );
        takes_[index].tangents_[0].normalize ( );

        std::cout << " calRecTagent " << takes_[index].keyframes_.size ( ) << std::endl;

}

void GLWidget::setPlay ( int index )
{

        if ( takes_[index].number_of_keyframes_ > 1 )
        {
                calculeTangents ( index );

                play_ = !play_;


                if ( !play_ )
                {
                        takes_[index].nextKeyframe_ = 0;
                }
                else
                {
                        sourcePosition_ = takes_[index].keyframes_[0];
                        targetPosition_ = takes_[index].keyframes_[1];

                        sourcePositionTangent_ = takes_[index].tangents_[0];
                        targetPositionTangent_ = takes_[index].tangents_[1];

                        float angular = sourcePosition_.angularDistance ( targetPosition_ );

                        //std::cout << " Angular Distance " << angular << std::endl;

                        takes_[index].nextKeyframe_++;

                        time_interval_ = 0.01;

                        time_steps_ = 0.0f * time_interval_;

                }
        }

}

void GLWidget::gameLooping ( )
{

        if ( zoom_start_ )
        {
                if ( zoom_increment_ > 0)
                        trackball_->increaseZoom ( 1.01 );
                else
                        trackball_->decreaseZoom( 1.01 );
                update();
        }

        //
        if ( play_ )
        {

                if ( time_steps_ <= 1.0 )
                {
//                        std::cout << "fps :" << time_steps_  << std::endl;
                        trackball_->setQuaternion(this->squad(sourcePosition_,sourcePositionTangent_,targetPositionTangent_,targetPosition_,time_steps_));
                        //trackball_->setQuaternion(this->slerp(sourcePosition_,targetPosition_,time_steps_,false));

                        //std::cout << " mTagent " << takes_[take_index_].keyframes_[takes_[take_index_].nextKeyframe_].coeffs() << std::endl;

                        //time_steps_ += time_interval_;

                        time_steps_ += time_interval_;

                        update();

                }
                else if ( takes_[take_index_].nextKeyframe_ < takes_[take_index_].number_of_keyframes_-1 )
                {
//                        std::cout << " nextKeyFrame " << nextKeyframe_ << std::endl;
//
//                        std::cout << " mTagent " << takes_[take_index_].tangents_[nextKeyframe_].coeffs() << std::endl;
//                        std::cout << " Rot " << keyframes_[nextKeyframe_].coeffs() << std::endl;


                        takes_[take_index_].nextKeyframe_++;

                        sourcePosition_ = targetPosition_;
                        targetPosition_ = takes_[take_index_].keyframes_[takes_[take_index_].nextKeyframe_];

                        sourcePositionTangent_ = targetPositionTangent_;
                        targetPositionTangent_ = takes_[take_index_].tangents_[takes_[take_index_].nextKeyframe_];

                        float angular = sourcePosition_.angularDistance(targetPosition_);

                        //std::cout << "Angular Distance : " << angular << std::endl;

                        time_interval_ = 0.01;
                                                                                                                                                        ;

                        time_steps_ = 0.0f*time_interval_;

                        //std::cout << " Angular Distance " << angular << std::endl;

//                        sourcePosition_.normalize();
//                        targetPosition_.normalize();
//
//                        sourcePositionTangent_.normalize();
//                        targetPositionTangent_.normalize();

                }
                else
                {

                        play_ = false;
                        takes_[take_index_].nextKeyframe_ = 0;
//                        std::cout << " Angular Distance " << angular << std::endl;
                }

        }
        update();

     fps++;

}

void GLWidget::fpsCounter ( )
{
        //std::cout << "fps :" << fps  << std::endl;
        fps = 0;

        delta_time.restart();
}
